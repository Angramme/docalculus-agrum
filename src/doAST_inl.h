


namespace gum {
    template<typename GUM_SCALAR, std::forward_iterator Iter, std::sentinel_for<Iter> Sen>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTreesI(Iter begin, Sen end){
        auto x = std::move(*begin);
        ++begin;
        if(begin == end) return std::move(x);
        return std::make_unique<ASTmult<GUM_SCALAR>>(std::move(x), std::move(productOfTreesI<GUM_SCALAR, Iter, Sen>(begin, end)));
    }

    template<typename GUM_SCALAR, typename Iterable>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Iterable& xs){
        auto ret = productOfTreesI<GUM_SCALAR, decltype(xs.begin()), decltype(xs.end())>(xs.begin(), xs.end());
        return std::move(ret);
    }

    template<typename GUM_SCALAR>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::initializer_list<std::unique_ptr<ASTtree<GUM_SCALAR>>> xs){
        auto ret = productOfTreesI<GUM_SCALAR, std::unique_ptr<ASTtree<GUM_SCALAR>>*>(xs.begin(), xs.end());
        return std::move(ret);
    }

    template<typename GUM_SCALAR>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::vector<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs){
        return productOfTrees<GUM_SCALAR, decltype(xs)>(xs);
    }

    template<typename GUM_SCALAR>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Set<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs){
        return productOfTrees<GUM_SCALAR, decltype(xs)>(xs);
    }


    template<typename GUM_SCALAR>
    INLINE
    std::string ASTtree<GUM_SCALAR>::toLatex(NameCounter* nameOccur) const {
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

    template<typename GUM_SCALAR>
    INLINE
    const ASTtree<GUM_SCALAR>& ASTsum<GUM_SCALAR>::term() const{
        return *_term_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const std::string& ASTsum<GUM_SCALAR>::var() const{
        return _var_;
    }

    template<typename GUM_SCALAR>
    INLINE
    const NameSet& ASTJointProba<GUM_SCALAR>::vars() const{
        return *_varnames_;
    }
}