namespace gum{
    using NameSet = Set<std::string>;

    template<typename GUM_SCALAR>
    std::tuple<CausalFormula<GUM_SCALAR>, Potential<GUM_SCALAR>, std::string> causalImpact(
        const CausalModel<GUM_SCALAR>& cm, 
        const NameSet& on,
        const NameSet& doing,
        const NameSet& knowing,
        const HashTable<std::string, NodeId>& values
    ) {
        auto total = NameSet();
        for(auto& i : on + doing + knowing)
            total.insert(cm.observationalBN().variable(cm.observationalBN().idFromName(i)).name());

        for(auto& k : values){
            if(total.contains(k.first)) continue;
            throw std::invalid_argument(k + " is not in the query arguments.");
        }

        if((on + doing + knowing).size() > 0)
            throw std::invalid_argument("The 3 parts of the query (on, doing, knowing) must not intersect!");

        const auto& [formula, potential, explanation] = _causalImpact(cm, on, doing, knowing);
        
        auto sv = potential.names();
        {
            Instantiation extract_values;
            for(const auto& t : values)
                if(sv.contains(t.first)) sv.chgVal(t.first, _getLabelIdx(cm.observationalBN(), t.first, t.second));
            potential = potential.extract(extract_values);
        }

        formula._setDoing(doing);
        formula._setKnowing(knowing);

        return std::make_tuple(formula, potential, explanation);
    }

    template<typename GUM_SCALAR>
    std::tuple<CausalFormula<GUM_SCALAR>, Potential<GUM_SCALAR>, std::string> _causalImpact(
        const CausalModel<GUM_SCALAR>& cm, 
        const NameSet& on,
        const NameSet& doing,
        const NameSet& knowing        
    ) {
        auto id_on = NodeSet();
        for(const auto& x : on) id_on.insert(x);
        auto id_doing = NodeSet();
        for(const auto& x : doing) id_doing.insert(x);
        auto id_knowing = NodeSet();
        for(const auto& x : knowing) id_knowing.insert(x);
        
        std::string explain = "";
        CausalFormula<GUM_SCALAR> ar;

        if(isDSep(cm, id_doing, id_on, id_knowing + cm.latentVariablesIds())){
            ar = CausalFormula(cm, ASTPosteriorProba(
                cm.causalBN(), on, knowing), on, doing, knowing);

            explain = "No causal effect of X on Y, because they are d-separated ";
            explain += "(conditioning on the observed variables if any).";
        }

        // front or back door
        if(id_doing.size() == 1 && on.size() == 1 && knowing.size() == 0){
            if(auto bd = cm.backDoor(*id_doing.begin(), *id_on.begin())){
                ar = CausalFormula(cm, getBackDoorTree(
                    cm, *doing.begin(), *on.begin(), bd.value), on, doing, knowing);
                explain = "backdoor ";
                for(const auto& i : bd.value) explain += cm.causalBN().variable(i).name();
                explain += " found.";
            }else if(auto fd = cm.frontDoor(*id_doing.begin(), *id_on.begin())){
                ar = CausalFormula(cm, getFrontDoorTree(
                    cm, *doing.begin(), *on.begin(), fd), on, doing, knowing);
                explain = "frontdoor ";
                for(const auto& i : fd.value) explain += cm.causalBN().variable(i).name();
                explain += " found.";
            }
        }

        if(explain != ""){
            auto adj = ar.eval();
            auto ret2 = std::vector< const DiscreteVariable* >();
            for(const auto& v : on + doing + knowing) 
                if(adj.names.contains(v)) ret2.push_back(v);
            return std::make_tuple(ar, adj.reorganize(ret2), explain);
        }
        
        if(knowing.size() == 0)
            ar = doCalculus(cm, on, doing);
        else
            ar = doCalculusWithObservation(cm, on, doing, knowing);

        auto adj = ar.eval();
        auto ret2 = std::vector< const DiscreteVariable* >();
        const auto lsum = on + doing + knowing;
        for(const auto& v : lsum) 
            if(adj.names.contains(v)) ret2.push_back(v);
        for(const auto& v : adj.names)
            if(!lsum.contains(v)) ret2.push_back(v);
        explain = "Do-calculus computations";
        return std::make_tuple(ar, adj.reorganize(ret2), explain);
    }

    template<typename GUM_SCALAR>
    CausalModel<GUM_SCALAR> counterfactualModel(
        const CausalModel<GUM_SCALAR>& cm, 
        const HashTable<std::string, NodeId>& profile, 
        const NameSet& whatif
    ) {
        NodeSet id_whatif;
        for(const auto& x : whatif) id_whatif.insert(cm.idFromName(x));

        // Step 1 : calculate the posterior probabilities of idiosyncratic nodes knowing the profil
        // whatif can be a string or a set of strings
        NodeSet orphans; //< nodes without parents in the causal model
        NodeSet nodes; //< nodes of the causal model
        for(const auto& x : cm.names()) nodes.insert(x.first);
        for(const auto& n : nodes){
            if(cm.parents(n).size() != 0) continue;
            orphans.insert(n);
        }

        // idiosyncratic factors (specific to and representative of the profile) are 
        // parentless - (whatif+latent variables)
        auto idiosyncratic = (orphans - id_whatif) - cm.latentVariablesIds();

        CausalModel ret = cm;
        auto& bn = ret.observationalBN();

        auto posteriors = HashTable<NodeId, Potential<GUM_SCALAR>>(); 
        auto ie = LazyPropagation(bn);
        ie.setEvidence(profile);
        ie.makeInference();
        for(const auto& factor : idiosyncratic)
            posteriors[factor] = ie.posterior(factor);

        for(const auto& factor : idiosyncratic)
            bn.cpt(factor).fillWith(posteriors[factor]);
        
        return ret;
    }

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> counterfactual(
        const CausalModel<GUM_SCALAR>& cm, 
        const HashTable<std::string, NodeId> profile, 
        const NameSet& on,
        const NameSet& whatif,
        const HashTable<std::string, NodeId>& values   
    ) {
        // step 1 and 2 : create the twin causal model
        auto twin = counterfactualModel(cm, profile, whatif);

        // step 3 : operate the intervention in the causal model based on bn
        Potential<GUM_SCALAR> adj = causalImpact(twin, on, whatif, NameSet(), values)[1];

        // adj is using variables from twin. We copy it in a Potential using variables of cm
        Potential<GUM_SCALAR> res;
        for(const auto& v : adj.value.names)
            res.add(cm.observationalBN().variableFromName(v));
        res.fillWith(adj);
        return res;
    }   
}