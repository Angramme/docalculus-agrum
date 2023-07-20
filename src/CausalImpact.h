
#ifndef _CAUSAL_IMPACT_H_
#define _CAUSAL_IMPACT_H_

#include "dSeparation.h"
#include "doAST.h"
#include "exceptions.h"
#include "CausalModel.h"
#include "CausalFormula.h"
#include "doCalculus.h"
#include "agrum/tools/core/set.h"
#include "agrum/tools/core/hashTable.h"
#include <agrum/tools/multidim/potential.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <string>
#include <tuple>
#include <stdexcept>


namespace gum{
    using NameSet = Set<std::string>;

    // TODO: add boundary (and connectedComponents) everywhere where there is parents or neighbours or children...
    // TODO: add GUM CONSTRUCTOR and DESCTRUCTOR MACROS...

    /**
     * @brief Determines the causal impact of interventions. Determines the causal impact
     *  of the interventions specified in ``doing`` on the single or list of variables ``on``
     *  knowing the states of the variables in ``knowing`` (optional). These last parameters 
     * is dictionary <variable name>:<value>. The causal impact is determined in the causal 
     * DAG ``cm``. This function returns a triplet with a latex format formula used to 
     * compute the causal impact, a potential representing the probability distribution of 
     * ``on`` given the interventions and observations as parameters, and an explanation of 
     * the method allowing the identification. If there is no impact, the joint probability
     *  of ``on`` is simply returned. If the impact is not identifiable the formula and the
     *  adjustment will be ``None`` but an explanation is still given. 
     *
     * @param cm the causal model
     * @param on variable name or variable names set of interest
     * @param doing the interventions
     * @param knowing the observations
     * @param values the values of interventions and observations
     * @return Tuple[CausalFormula,pyAgrum.Potential,str] the CausalFormula, the computation,
     *       the explanation
     * @throws HedgeException
     */
    template<typename GUM_SCALAR>
    std::tuple<CausalFormula<GUM_SCALAR>, Potential<GUM_SCALAR>, std::string> causalImpact(
        const CausalModel<GUM_SCALAR>& cm, 
        const NameSet& on,
        const NameSet& doing,
        const NameSet& knowing,
        const HashTable<std::string, NodeId>& values
    );


    /**
     * @brief Determines the causal impact of interventions. Determines the causal
     * impact of the interventions specified in ``doing`` on the single or
     * list of variables ``on`` knowing the states of the variables in
     * ``knowing`` (optional). The causal impact is determined in the causal
     * DAG ``cm``. This function returns a triplet with a latex format
     * formula used to compute the causal impact, a potential representing
     * the probability distribution of ``on`` given the interventions and
     * observations as parameters, and an explanation of the method allowing
     * the identification. If there is no impact, the joint probability of
     * ``on`` is simply returned. If the impact is not identifiable the
     * formula and the adjustment will be ``None`` but an explanation is
     * still given. 
     *
     * @param cm the causal model
     * @param on  targeted variable(s)
     * @param doing interventions
     * @param knowing observations
     * @return Tuple[CausalFormula,Potential,str] the latex representation,
     *     the computation, the explanation
     * @throws HedgeException
     */
    template<typename GUM_SCALAR>
    std::tuple<CausalFormula<GUM_SCALAR>, Potential<GUM_SCALAR>, std::string> _causalImpact(
        const CausalModel<GUM_SCALAR>& cm, 
        const NameSet& on,
        const NameSet& doing,
        const NameSet& knowing        
    );

    /**
     * @brief Determines the estimation of the twin model following the three steps
     * algorithm from "The Book Of Why" (Pearl 2018) chapter 8 page 253. This
     * is done according to the following algorithm: 
     * 
     *     -Step 1: calculate the posterior probabilities of idiosyncratic nodes
     * (parentless nodes - whatif-latent variables) in the BN with which we created 
     * the causal model with "profile" as evidence. 
     * 
     *     -Step 2 : We replace in the original BN the prior probabilities of 
     * idiosyncratic nodes with potentials calculated in step 1 (it will spread to
     * the causal model) This function returns the twin CausalModel.
     *
     * @param cm CausalModel
     * @param profile evidence
     * @param whatif idiosyncratic nodes
     * @return CausalModel the twin CausalModel
     */ 
    template<typename GUM_SCALAR>
    CausalModel<GUM_SCALAR> counterfactualModel(
        const CausalModel<GUM_SCALAR>& cm, 
        const HashTable<std::string, NodeId>& profile, 
        const NameSet& whatif
    );

     /**
     * @brief Determines the estimation of a counterfactual query following the the
     * three steps algorithm from "The Book Of Why" (Pearl 2018) chapter 8
     * page 253. Determines the estimation of the counterfactual query: Given
     * the "profile" (dictionary <variable name>:<value>),what would
     * variables in "on" (single or list of variables) be if variables in
     * "whatif" (single or list of variables) had been as specified in
     * "values" (dictionary <variable name>:<value>)(optional). This is done
     * according to the following algorithm:
     *  
     *      -Step 1-2: compute the twin causal model 
     * 
     *      -Step 3 : determine the causal impact of the
     * interventions specified in "whatif" on the single or list of variables
     * "on" in the causal model. This function returns the potential
     * calculated in step 3, representing the probability distribution of
     * "on" given the interventions "whatif", if it had been as specified in
     * "values" (if "values" is omitted, every possible value of "whatif")
     *
     * @param cm CausalModel :
     * @param profile evidence
     * @param on the variable(s) of interest
     * @param whatif idiosyncratic nodes
     * @param values values for certain variables in whatif.
     * @return the computed counterfactual impact
     * @throws HedgeException
     */
    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> counterfactual(
        const CausalModel<GUM_SCALAR>& cm, 
        const HashTable<std::string, NodeId> profile, 
        const NameSet& on,
        const NameSet& whatif,
        const HashTable<std::string, NodeId>& values   
    );
}

#include "CausalImpact_tpl.h"
#endif