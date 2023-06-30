    
#include "CausalFormula.h"

namespace gum{
    template<typename GUM_SCALAR>
    CausalFormula<GUM_SCALAR>::CausalFormula(
        const CausalModel<GUM_SCALAR>& cm, 
        const ASTtree<GUM_SCALAR>& root, 
        const std::set<std::string>& on, 
        const std::set<std::string>& doing, 
        const std::set<std::string>& knowing
    ) 
        : _cm_(cm), _root_(root), _on_(on), _doing_(doing), _knowing_(knowing)
    {}

    template<typename GUM_SCALAR>
    std::string CausalFormula<GUM_SCALAR>::_print_(const std::string& pre) const{
        return _root_._print_(pre);
    }

    template<typename GUM_SCALAR>
    const CausalModel<GUM_SCALAR>& CausalFormula<GUM_SCALAR>::cm() const {
        return _cm_;
    }
    template<typename GUM_SCALAR>
    const ASTtree<GUM_SCALAR>& CausalFormula<GUM_SCALAR>::root() const {
        return _root_;
    }

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> CausalFormula<GUM_SCALAR>::eval() const{
        return root().eval(cm().observationalBN());
    }
}
