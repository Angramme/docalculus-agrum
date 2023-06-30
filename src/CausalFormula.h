
#ifndef CAUSAL_FORMULA_H
#define CAUSAL_FORMULA_H

#include <string>
#include <set>

#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/multidim/potential.h>

#include "CausalModel.h"
#include "doAST.h"

namespace gum{

    /**
     * @brief Represents a causal query in a causal model. 
     * The query is encoded as a CausalFormula that can 
     * be evaluated in the causal model :
     *  $P(on|knowing, \\overhook (doing))$
     *
     * @param cm CausalModel : the causal model
     * @param root ASTtree : the syntax tree
     * @param on str|Set[str] : the variable or the set of variables of interest
     * @param doing str|Set[str] : the intervention variable(s)
     * @param knowing None|str|Set[str] : the observation variable(s)
     */
    template<typename GUM_SCALAR>
    class CausalFormula{
    private:
        CausalModel<GUM_SCALAR> _cm_;
        ASTtree<GUM_SCALAR> _root_;
        std::set<std::string> _on_;
        std::set<std::string> _doing_;
        std::set<std::string> _knowing_;

    public:
        /**
         * @brief Represents a causal query in a causal model. 
         * The query is encoded as an CausalFormula that can 
         * be evaluated in the causal model :
         *  $P(on|knowing, \\overhook (doing))$
         *
         * @param cm CausalModel : the causal model
         * @param root ASTtree : the syntax tree
         * @param on str|Set[str] : the variable or the set of variables of interest
         * @param doing str|Set[str] : the intervention variable(s)
         * @param knowing None|str|Set[str] : the observation variable(s)
         */
        CausalFormula(
            const CausalModel<GUM_SCALAR>& cm, 
            const ASTtree<GUM_SCALAR>& root, 
            const std::set<std::string>& on, 
            const std::set<std::string>& doing, 
            const std::set<std::string>& knowing);

        /**
         * @brief Prints the object
         *
         * @param pre  : a prefix for each line of the string representation
         * @return str the string version of the CausalFormula
         */
        std::string _print_(const std::string& pre) const;

         /**
         * @brief Returns a string representing the query compiled by this Formula. If values, the query is annotated with the values in the dictionary.        
         *
         * @param values None|Dict[str,str] : the values to add in the query representation
         * @return str the LaTeX representation of the causal query for this CausalFormula
         */
        std::string latexQuery(const HashTable<std::string, std::string>& values) const;

        const CausalModel<GUM_SCALAR>& cm() const; 
        const ASTtree<GUM_SCALAR>& root() const;

        /**
         * @brief Compute the Potential from the CausalFormula over vars using cond as value for others variables
         *
        * @return Potential The resulting distribution
        */
        Potential<GUM_SCALAR> eval() const;
    };
}


#include "CausalFormula_tpl.h"

#endif