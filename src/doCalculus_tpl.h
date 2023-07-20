

namespace gum{
    
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculus(const CausalModel<GUM_SCALAR>& cm, const NameSet& on, const NameSet& doing) {
        return CausalFormula(cm, identifyingIntervention(cm, on, doing), on, doing);
    }

   
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculus(const CausalModel<GUM_SCALAR>& cm, const std::string& on, const std::string& doing) {
        return CausalFormula(cm, identifyingIntervention(cm, Set<std::string>({on}), Set<std::string>({doing})), on, doing);
    }

    
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculusWithObservation(const CausalModel<GUM_SCALAR>& cm, const Set<std::string>& on, const NameSet& doing, const NameSet& knowing) {
        if(knowing.size() == 0) return doCalculus(cm, on, doing);

        auto iDoing = Set<NodeId>({});
        for(auto x : doing) iDoing.insert(cm.idFromName(x));
        auto iOn = Set<NodeId>({});
        for(auto x : on) iOn.insert(cm.idFromName(x));
        auto iKnowing = Set<NodeId>({});
        for(auto x : knowing) iKnowing.insert(cm.idFromName(x));

        auto removedArcs = std::vector<std::pair<NodeId, NodeId>>();
        for(const auto& id : iDoing){
            for(const auto& p : cm.parents(id)){
                cm.eraseCausalArc(p, id);
                removedArcs.push_back(make_pair(p, id));
            }
        }
        for(const auto& id : iKnowing){
            for(const auto& c : cm.children(id)){
                cm.eraseCausalArc(id, c);
                removedArcs.push_back(make_pair(id, c));
            }
        }
        auto rg = dSep_reduce(cm, iDoing + iOn + iKnowing);
        for(const auto& arc : removedArcs){
            cm.addCausalArc(arc.head, arc.tail);
        }
        for(const auto& id : iKnowing){
            if(isDSep(rg, Set({id}), iOn, iDoing + (iKnowing - Set({id})))){
                try{
                    return doCalculusWithObservation(cm, on, doing + Set({cm.names()[id]}), knowing - Set({cm.names()[id]}));
                }catch(const gum::HedgeException& h){}
            }
        }
        auto p = doCalculus(cm, on + knowing, doing);
        auto q = doCalculus(cm, knowing, doing);

        return CausalFormula(cm, ASTdiv(p.root(), q.root()), on, doing, knowing);
    }
    
    void __undiComponent(UndiGraph& g, NodeId n, NodeSet& se){
        for(const auto& i : g.neighbours(n)){
            if(se.contains(i)) continue;
            se.insert(i);
            __undiComponent(g, i, se);
        }
    };

    template<typename GUM_SCALAR>
    std::vector<Set<NodeId>> _cDecomposition(const CausalModel<GUM_SCALAR>& cm){
        auto undi = UndiGraph();
        auto s = cm.nodes() - cm.latentVariablesIds();
        for(const auto& n : s) undi.addNodeWithId(n);
        for(const auto& latent : cm.latentVariablesIds()){
            auto chils = cm.children(latent);
            for(auto&& arc : combinations(chils, 2)){
                undi.addEdge(arc[0], arc[1]);
            }
        }

        auto components = std::vector<Set<NodeId>>();
        while(s.size() != 0){
            auto c = *s.begin();
            s.erase(s.begin());
            auto sc = Set({c});
            __undiComponent(undi, c, sc);
            s -= sc;
            components.push_back(sc);
        }
        return components;
    }


    template<typename GUM_SCALAR>
    std::vector<int> _topological_sort(const CausalModel<GUM_SCALAR>& cm) {
        auto lt = cm.latentVariablesIds();
        auto dc = HashTable<NodeId, size_t>();
        for(const auto& i : cm.nodes() - lt) dc[i] = (cm.parents(i) - lt).size();

        auto order = std::vector<NodeId>();
        while(dc.size() != 0){
            auto rem = NodeSet();
            for(auto i : dc){
                if(dc[i] != 0) continue;
                order.push_back(i);
                rem.insert(i);
            }
            for(auto r : rem){
                for(auto c : cm.children(r)) dc[c] -= 1;
                dc.erase(r);
            }
        }
        return order;
    }


    
    template<typename GUM_SCALAR>
    std::unique_ptr<ASTtree<GUM_SCALAR>> identifyingIntervention(
        const CausalModel<GUM_SCALAR>& cm, const NameSet& Y, const NameSet& X, std::unique_ptr<ASTtree<GUM_SCALAR>> P)
        { 
        // TODO: giga-tester ce truc
        auto iX = Set<NodeId>();
        for(const auto& x : X) iX.insert(cm.idFromName(x));
        auto iY = Set<NodeId>();
        for(const auto& x : Y) iY.insert(cm.idFromName(x));
        auto iV = cm.nodes() - cm.latentVariablesIds();
        auto V = Set<std::string>();
        for(const auto& x : iV) V.insert(cm.names()[x]);

        // step 1 --------------------------
        if(X.size() == 0){
            auto vy = V - Y;
            if(vy.size() != 0){
                return ASTsum(vy.begin(), vy.end(), std::move(P));
            }
            return P;
        }

        // step 2 --------------------------
        auto iAnY = NodeSet();
        for(const auto& i : iY){
            ancestor(i, cm, iAnY);
        }
        iAnY += iY;
        auto AnY = Set<std::string>();
        for(auto x : iAnY) AnY.insert(cm.names()[x]);

        if(cm.nodes().size() != AnY.size()){
            auto ivAny = iV - iAnY;
            auto vAny = Set<std::string>();
            for(const auto& x : ivAny) vAny.insert(cm.names()[x]);
            P = ASTsum(vAny, std::move(P));
            return identifyingIntervention(inducedCausalSubModel(cm, iAnY), Y, X + AnY, P);
        }

        // step 3 -------------------------
        auto rmArcs = Set<std::pair<NodeId, NodeId>>();

        for(const auto& e : iX){
            for(const auto& p : cm.parents(e)){
                cm.eraseCausalArc(p, e);
                rmArcs.insert(make_pair(p, e));
            }
        }
        
        auto ianY = NodeSet();
        for(const auto& i : iY){
            ancestor(i, cm, ianY);
        }
        ianY += iY;
        auto iW = (iV - iX) - ianY;

        for(const auto& arc : rmArcs){
            cm.addCausalArc(arc.head, arc.tail);
        }

        if(iW.size() != 0){
            auto W = Set<std::string>();
            for(const auto& x : iW) W.insert(cm.names()[x]);
            return identifyingIntervention(cm, Y, X + W, std::move(P));
        }

        auto gvx = inducedCausalSubModel(cm, iV - iX);
        auto cd = std::vector<Set<std::string>>();
        auto icd = _cDecomposition(gvx);
        for(const auto& s : icd){
            auto temp = Set<std::string>();
            for(const auto& x : s) temp.insert(cm.names()[x]);
            cd.push_back(temp);
        }

        // step 4 ----------------------------------
        if(cd.size() > 1){
            auto t = identifyingIntervention(cm, cd[0], V - cd[0], std::move(P));
            for(auto si = ++cd.begin(); si != cd.end(); ++si){
                t = ASTmult<GUM_SCALAR>(identifyingIntervention(cm, si, V - *si, std::make_unique<ASTtree<GUM_SCALAR>>(*P)), t);
            }
            auto vyx = V - (X + Y);
            
            if(vyx.size() == 0) return t;
            return ASTsum(vyx.begin(), vyx.end(), std::move(t));
        }

        auto S = cd[0];
        auto iS = icd[0];
        auto cdg = _cDecomposition(cm);

        // step 5 ---------------------------------
        if(cdg.size() == 1 && cdg[0].size() == V.size()){
            auto ss = std::stringstream();
            ss << "Hedge Error: G=" << V << ", G[S]=" << S;
            throw HedgeException(ss.str().c_str(), V);
        }


        // step 6 --------------------------------
        auto gs = inducedCausalSubModel(cm, iS);
        if(std::find(cdg.begin(), cdg.end(), gs.nodes() - gs.latentVariablesIds()) != cdg.end()){
            auto prb = std::vector<ASTtree<GUM_SCALAR>>();
            auto to = _topological_sort(cm);
            for(const auto& v : S){
                auto vpi = std::find(to.begin(), to.end(), cm.idFromName(v));
                if(vpi == to.begin() || vpi == to.end()){
                    prb.push_back(ASTJointProba({v}));
                }else{
                    if(P == nullptr){
                        auto nvpi = Set<std::string>();
                        for(auto i=to.begin(); i!=vpi; ++i) nvpi.insert(cm.names()[*i]);
                        prb.push_back(ASTPosteriorProba(cm.causalBN(), Set({v}), nvpi));
                    }else{
                        prb.push_back(ASTdiv(std::make_unique(P), ASTsum(v, std::make_unique(P))));
                    }
                }
            }

            auto prod = productOfTrees(prb);
            if((S - Y).size() == 0) return prod;

            auto ilsy = imap([&](std::string& x){ return cm.idFromName(x); }, S - Y);
            return ASTsum(ilsy.begin(), ilsy.end(), prod);
        }

        // step 7 ---------------------------
        for(const auto& ispr : cdg){
            if(iS > ispr) continue;
            auto spr = imap([&](NodeId i){ return cm.names()[i]; }, ispr);
            auto prb = std::vector<ASTtree<GUM_SCALAR>>();
            auto top = _topological_sort(cm);

            for(const auto& v : spr){
                auto vpi = std::find(top.begin(), top.end(), cm.idFromName(v));
                if(vpi == top.begin() || vpi == top.end()){
                    prb.push_back(ASTjointProba({v}));
                }else{
                    auto nvpi = Set<std::string>();
                    for(auto i=top.begin(); i!=vpi; ++i) nvpi.insert(cm.names()[*i]);
                    prb.push_back(ASTPosteriorProba(cm.causalBN(), Set({v}), nvpi));
                }
            }
            auto P = productOfTrees(prb);
            return identifyingIntervention(inducedCausalSubModel(cm, ispr), Y, X + Set(spr.begin(), spr.end()), P);
        }

        return nullptr;
    }

    
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR> getBackDoorTree(const CausalModel<GUM_SCALAR>& cm, const std::string& x, const std::string& y, const NodeSet& zset) {

        auto zp = std::vector<std::string>();
        for(const auto& i : zset) zp.push_back(cm.names()[i]);
        auto zps = Set(zp);
        zps.insert(x);
        return ASTsum(zp, ASTmult(ASTposteriorProba(cm.causalBN(), Set({y}), zps), 
            ASTJointProba<GUM_SCALAR>(zp)));
    }

    
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR> getFrontDoorTree(const CausalModel<GUM_SCALAR>& cm,const std::string& x,const std::string& y,const NodeSet& zset) {
        auto zp = std::vector<std::string>();
        for(const auto& i : zset) zp.push_back(cm.names()[i]);
        auto zps = Set(zp);
        zps.insert(x);
        return ASTsum(zp, ASTmult(ASTposteriorProba(cm.causalBN(), Set(zp), Set({x})), 
            ASTsum({x}, ASTmult(ASTposteriorProba(cm.causalBN(), Set({y}), zps)), ASTJointProba<GUM_SCALAR>({x}))));
    }
}