
#ifndef GUM_DO_CALCULUS_H
#define GUM_DO_CALCULUS_H


#include "CausalModel.h"
#include "CausalFormula.h"
#include "exceptions.h"
#include "dSeparation.h"
#include "agrum/tools/graphs/undiGraph.h"

#include <sstream>

namespace gum{

    /**
     * @brief Compute the CausalFormula for computing an impact analysis
     *  given the causal model, the observed variables and the variable on 
     * which there will be intervention. Note that there is no value neither 
     * for ``on`` nor for ``doing`` variables
     * 
     * @param cm the causal model
     * @param on the variables of interest
     * @param doing the interventions
     * @return CausalFormula for computing this causal impact
     */
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculus(const CausalModel<GUM_SCALAR>& cm, const NameSet& on, const NameSet& doing) ;

    /**
     * @brief Compute the CausalFormula for computing an impact analysis
     *  given the causal model, the observed variables and the variable on 
     * which there will be intervention. Note that there is no value neither 
     * for ``on`` nor for ``doing`` variables
     * 
     * @param cm the causal model
     * @param on the variable of interest
     * @param doing the interventions
     * @return CausalFormula for computing this causal impact
     */
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculus(const CausalModel<GUM_SCALAR>& cm, const std::string& on, const std::string& doing) ;

    /**
     * @brief Compute the CausalFormula for an impact analysis given the causal model, 
     * the observed variables and the variable on which there will be intervention. 
     * 
     * @param cm the causal model
     * @param on the variables of interest
     * @param doing the interventions
     * @param knowing the observations
     * @return CausalFormula if possible, returns the formula to compute this intervention
     * @throws HedgeException, UnidentifiableException  if this calculus is not possible
     */
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR> doCalculusWithObservation(const CausalModel<GUM_SCALAR>& cm, const Set<std::string>& on, const NameSet& doing, const NameSet& knowing) ;


    /**
     * @brief Following Shpitser, Ilya and Judea Pearl. 'Identification 
     * of Conditional Interventional Distributions.' UAI2006 and 'Complete 
     * Identification Methods for the Causal Hierarchy' JMLR 2008
     * 
     * @param cm the causal model
     * @param Y The variables of interest (named following the paper)
     * @param X The variable of intervention (named following the paper)
     * @param P The ASTtree representing the calculus in construction
     * @return ASTtree the ASTtree representing the calculus
     */
    template<typename GUM_SCALAR>
    std::unique_ptr<ASTtree<GUM_SCALAR>> identifyingIntervention(
        const CausalModel<GUM_SCALAR>& cm, const NameSet& Y, const NameSet& X, std::unique_ptr<ASTtree<GUM_SCALAR>> P);

    /**
     * @brief Create a CausalFormula representing a backdoor zset from x to y in the causal mode lcm
     * 
     * @param cm causal model
     * @param x impacting node
     * @param y impacted node
     * @param zset backdoor set
     * @return ASTtree the ASTtree for the backoor criteria
     */
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR> getBackDoorTree(const CausalModel<GUM_SCALAR>& cm, const std::string& x, const std::string& y, const NodeSet& zset) ;

    /**
     * @brief Create an AdsT representing a frontdoor zset from x to y in the causal model
     * 
     * @param cm causal model
     * @param x impacting node
     * @param y impacted node
     * @param zset frontdoor set
     * @return ASTtree the ASTtree for the frontdoot critreroia
     */
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR> getFrontDoorTree(const CausalModel<GUM_SCALAR>& cm,const std::string& x,const std::string& y,const NodeSet& zset);

}


#include "doCalculus_tpl.h"
#endif