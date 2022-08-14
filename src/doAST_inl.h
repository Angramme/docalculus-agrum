
#include <agrum/agrum.h>
#include "doAST.h"

namespace gum {

    template<typename GUM_SCALAR>
    INLINE
    std::string ASTtree<GUM_SCALAR>::toLatex(const NameCounter& nameOccur) const {
        return this->_to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    INLINE
    const ASTtree<GUM_SCALAR>& ASTBinaryOp<GUM_SCALAR>::op1() const {
        return *_op1_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const ASTtree<GUM_SCALAR>& ASTBinaryOp<GUM_SCALAR>::op2() const {
        return *_op2_;
    }

    template<typename GUM_SCALAR>
    INLINE
    ASTtree<GUM_SCALAR>& ASTBinaryOp<GUM_SCALAR>::op1() {
        return *_op1_;
    }

    template<typename GUM_SCALAR>
    INLINE
    ASTtree<GUM_SCALAR>& ASTBinaryOp<GUM_SCALAR>::op2() {
        return *_op2_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const Set<std::string>& ASTPosteriorProba<GUM_SCALAR>::vars() const {
        return _varset_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const Set<std::string>& ASTPosteriorProba<GUM_SCALAR>::knw() const {
        return _knwset_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const Set<std::string>& ASTPosteriorProba<GUM_SCALAR>::bn() const {
        return _bn_;
    }
}