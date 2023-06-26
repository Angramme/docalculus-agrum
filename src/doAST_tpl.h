#include "doAST.h"
#include <memory>
#include <vector>
#include <iterator>
#include <sstream>
#include <agrum/tools/core/set.h>
#include <agrum/BN/inference/lazyPropagation.h>


template<typename GUM_SCALAR>
std::ostream& operator<< ( std::ostream& outs, const gum::ASTtree<GUM_SCALAR>& p ){
    p._print_(outs, 0);
    return outs;
}
namespace gum{
    template<typename GUM_SCALAR>
    std::string ASTtree<GUM_SCALAR>::_continueNextLine_ = "| ";
    template<typename GUM_SCALAR>
    bool ASTtree<GUM_SCALAR>::_verbose_ = false;
    
    template<typename GUM_SCALAR>
    void ASTtree<GUM_SCALAR>::setVerbosity(bool value){
        _verbose_ = value;
    }

    template<typename GUM_SCALAR>
    bool ASTtree<GUM_SCALAR>::getVerbosity() {
        return _verbose_;
    }
    
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR>::ASTtree(const std::string& typ)
        : _type_(typ)
    {}

    template<typename GUM_SCALAR>
    constexpr std::string ASTtree<GUM_SCALAR>::_latext_var_present_(
        const std::string& src, NameCounter* nameOccur)
    {
        const size_t nbr = nameOccur ? std::max((size_t)0, (*nameOccur)[src] - 1) : 0;
        std::string ret = src;
        for(size_t i=0; i<nbr; i++) ret += "'";
        return ret;
    }

    template<typename GUM_SCALAR> 
    // template<std::input_iterator Iter, std::sentinel_for<Iter> Sen>
    template<typename Iter>
    constexpr std::vector<std::string> ASTtree<GUM_SCALAR>::_latext_var_present_i_(
        Iter b_src, Iter e_src, NameCounter* nameOccur)
    {
        auto vs = std::vector<std::string>(std::distance(b_src, e_src));
        size_t i = 0;
        for(Iter c = b_src; c != e_src; c+=1) vs[i++] = _latext_var_present_(*c, nameOccur);
        return vs;
    }

    template<typename GUM_SCALAR>
    constexpr std::vector<std::string> ASTtree<GUM_SCALAR>::_latext_var_present_(
        const std::vector<std::string>& src, NameCounter* nameOccur)
    { return _latext_var_present_i_(src.cbegin(), src.cend(), nameOccur); }

    template<typename GUM_SCALAR>
    constexpr std::vector<std::string> ASTtree<GUM_SCALAR>::_latext_var_present_(
        const Set<std::string>& src, NameCounter* nameOccur)
    { return _latext_var_present_i_(src.cbegin(), src.cend(), nameOccur); }

    template<typename GUM_SCALAR>
    ASTBinaryOp<GUM_SCALAR>::ASTBinaryOp(const std::string& typ, std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : ASTtree<GUM_SCALAR>(typ), _op1_(std::move(op1)), _op2_(std::move(op2))
    {}

    template<typename GUM_SCALAR>
    void ASTBinaryOp<GUM_SCALAR>::_print_(std::ostream& outs, int indent) const {
        for(int i=0; i<indent; i++) outs << this->_continueNextLine_;
        outs << this->type() << std::endl;
        this->op1()._print_(outs, indent + 1);
        outs << std::endl;
        this->op2()._print_(outs, indent + 1);
    }

    template<typename GUM_SCALAR>
    ASTplus<GUM_SCALAR>::ASTplus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : ASTBinaryOp<GUM_SCALAR>("+", std::move(op1), std::move(op2))
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTplus<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->_verbose_) cout << "EVAL operation + " << endl;
        auto res = this->_op1_->eval(bn) + this->_op2_->eval(bn);
        if(this->_verbose_) cout << "END OF EVAL operation + : " << res << endl;
        return res;
    }

    template<typename GUM_SCALAR>
    std::string ASTplus<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        return this->_op1_->_to_latex_(nameOccur) + "+" + this->_op2_->_to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    std::string ASTplus<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return "\\left("+_to_latex_(nameOccur)+"\\right)";
    }

    template<typename GUM_SCALAR>
    ASTminus<GUM_SCALAR>::ASTminus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : ASTBinaryOp<GUM_SCALAR>("-", std::move(op1), std::move(op2))
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTminus<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->_verbose_) cout << "EVAL operation - " << endl;
        auto res = this->_op1_->eval(bn) - this->_op2_->eval(bn);
        if(this->_verbose_) cout << "END OF EVAL operation - : " << res << endl;
        return res;
    }

    template<typename GUM_SCALAR>
    std::string ASTminus<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        return this->_op1_->_to_latex_(nameOccur) + "-" + this->_op2_->_to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    std::string ASTminus<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return "\\left("+_to_latex_(nameOccur)+"\\right)";
    }

    template<typename GUM_SCALAR>
    ASTmult<GUM_SCALAR>::ASTmult(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : ASTBinaryOp<GUM_SCALAR>("*", std::move(op1), std::move(op2))
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTmult<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->_verbose_) cout << "EVAL operation * " << endl;
        auto res = this->_op1_->eval(bn) * this->_op2_->eval(bn);
        if(this->_verbose_) cout << "END OF EVAL operation * : " << res << endl;
        return res;
    }

    template<typename GUM_SCALAR>
    std::string ASTmult<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        return this->_op1_->_to_latex_(nameOccur) + "\\cdot" + this->_op2_->_to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    std::string ASTmult<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return _to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    ASTdiv<GUM_SCALAR>::ASTdiv(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : ASTBinaryOp<GUM_SCALAR>("/", std::move(op1), std::move(op2))
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTdiv<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->_verbose_) cout << "EVAL operation / " << endl;
        auto res = this->_op1_->eval(bn) / this->_op2_->eval(bn);
        if(this->_verbose_) cout << "END OF EVAL operation / : " << res << endl;
        return res;
    }

    template<typename GUM_SCALAR>
    std::string ASTdiv<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        return " \\frac{" + this->_op1_->_to_latex_(nameOccur) + "}{" + this->_op2_->_to_latex_(nameOccur) + "}";
    }

    template<typename GUM_SCALAR>
    std::string ASTdiv<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return _to_latex_(nameOccur);
    }

    // helper function so that _knwset_ can stay const
    template<typename GUM_SCALAR>
    constexpr Set<std::string> _ASTPosteriorProba_init_knwset(
        const BayesNet<GUM_SCALAR>& bn, 
        const Set<std::string>& varset, 
        const Set<std::string>& knwset)
    {
        auto _knwset_ = Set<std::string>();
        for(const auto& i : bn.minimalCondSet(varset, knwset)){
            _knwset_.insert(bn.variable(i).name());
        }
        return _knwset_;
    }

    template<typename GUM_SCALAR>
    ASTPosteriorProba<GUM_SCALAR>::ASTPosteriorProba(
        const std::shared_ptr<BayesNet<GUM_SCALAR>> bn, 
        const std::shared_ptr<Set<std::string>> varset, 
        const std::shared_ptr<Set<std::string>> knwset)
        : ASTtree("_posterior_"), _bn_(bn), _varset_(varset), _knwset_(_ASTPosteriorProba_init_knwset(*bn, *varset, *knwset))
    {};

    void ___sort_comma_push_stream(std::ostream& outs, const Set<std::string>& VS);
    void ___sort_comma_push_stream(std::ostream& outs, const std::vector<std::string>& VS);

    template<typename GUM_SCALAR>
    void ASTPosteriorProba<GUM_SCALAR>::_print_(std::ostream& outs, int indent) const {
        for(int i=0; i<indent; i++) outs << this->_continueNextLine_;
        outs << "P(";
        ___sort_comma_push_stream(outs, vars());
        if(knw().size() > 0){
            outs << "|";
            ___sort_comma_push_stream(outs, knw());
        }
        outs << ")";
    }

    template<typename GUM_SCALAR>
    std::string ASTPosteriorProba<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        auto ss = std::stringstream();
        ss << "P\\left(";
        ___sort_comma_push_stream(ss, _latext_var_present_(vars(), nameOccur));
        if(knw().size() > 0){
            ss << "\\mid";
            ___sort_comma_push_stream(ss, _latext_var_present_(knw(), nameOccur));
        } 
        ss << "\\right)";

        return ss;
    }

    template<typename GUM_SCALAR>
    std::string ASTPosteriorProba<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return _to_latex_(nameOccur);
    }

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTPosteriorProba<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->getVerbosity()) cout << "EVAL $" << _to_latex_({}) << "$ in context" << std::endl; 
        auto ie = LazyPropagation(bn);

        Potential<GUM_SCALAR>* p = nullptr;

        if(vars().size() == 1){
            const auto& x = *vars().begin();
            const auto ix = bn.idFromName(x);
            const auto set = decltype(this->_knwset_)();
            for(const auto& i : bn.parents(ix))
                set.insert(bn.variable(i).name());
            if(set == knw()) p = &bn.cpt(ix);
        }
        if(p == nullptr){
            if(knw().size() == 0){
                ie.addJointTarget(vars());
                ie.makeInference();
                p = &ie.jointPosterior(vars());
            }else{
                ie.addJointTarget(vars() + knw());
                ie.makeInference();
                p = ie.jointPosterior(vars() + knw()) / ie.jointPosterior(knw());
            }
        }

//     # res = p.extract({k: v for k, v in context.todict().items() if k in self.vars + self.knw})

        if(this->getVerbosity()) cout << "END OF EVAL $" << _to_latex_({}) << "$ : " << p << std::endl;

        return p;
    } 

    template<typename GUM_SCALAR>
    ASTJointProba<GUM_SCALAR>::ASTJointProba(const std::shared_ptr<NameSet> varNames)
        : ASTtree<GUM_SCALAR>("_joint_"), _varnames_(varNames) 
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTJointProba<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const {
        using namespace std;
        if(this->getVerbosity()) cout << "EVAL $" << _to_latex_() << "$ in context" << endl;
        auto ie = LazyPropagation<GUM_SCALAR>(&bn);
        Potential<GUM_SCALAR> res;
        auto varIds = NodeSet();
        for(const auto& x : vars()) varIds.insert(bn.idFromName(x)); 
        if(vars().size() > 1){
            ie.addJointTarget(varIds);
            ie.makeInference();
            res = ie.jointPosterior(varIds);
        }else{
            const auto name = *vars().begin();
            ie.makeInference();
            res = ie.posterior(name);
        }

        if(this->getVerbosity()) cout << "END OF EVAL $" << _to_latex_() << "$ : " << res << endl;

        return res;
    }

    template<typename GUM_SCALAR>
    void ASTJointProba<GUM_SCALAR>::_print_(std::ostream& outs, int indent) const {
        for(int i=0; i<indent; i++) outs << this->_continueNextLine_;
        outs << "joint P(";
        ___sort_comma_push_stream(outs, vars());
        outs << ")";
    }
    
    template<typename GUM_SCALAR>
    std::string ASTJointProba<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const {
        auto ss = std::stringstream();
        ss << "P\\left(";
        ___sort_comma_push_stream(ss, this->_latext_var_present_(vars(), nameOccur));
        ss << "\\right)";
        return ss.str();
    }
    
    template<typename GUM_SCALAR>
    std::string ASTJointProba<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const {
        return _to_latex_(nameOccur);
    }


    template<typename GUM_SCALAR>
    ASTsum<GUM_SCALAR>::ASTsum(const std::string& var, std::unique_ptr<ASTtree<GUM_SCALAR>> term)
        : ASTsum(var.cbegin(), var.cend(), std::move(term))
    {}

    template<typename GUM_SCALAR>
    ASTsum<GUM_SCALAR>::ASTsum(std::initializer_list<std::string> var, std::unique_ptr<ASTtree<GUM_SCALAR>> term)
        // : ASTtree<GUM_SCALAR>("_sum_")
        : ASTsum(var.begin(), var.end(), std::move(term))
    {}

    template<typename GUM_SCALAR>
    template<std::input_iterator Iter, std::sentinel_for<Iter> Sen>
    ASTsum<GUM_SCALAR>::ASTsum(Iter b_var, Sen e_var, std::unique_ptr<ASTtree<GUM_SCALAR>> term)
        : ASTtree<GUM_SCALAR>("_sum_"), _var_(*b_var), 
        _term_(b_var+1==e_var ? std::move(term) : std::make_unique<ASTsum>(b_var+1, e_var, std::move(term)))
    {}

    template<typename GUM_SCALAR>
    Potential<GUM_SCALAR> ASTsum<GUM_SCALAR>::eval(const BayesNet<GUM_SCALAR>& bn) const{
        using namespace std;
        if(this->getVerbosity()) cout << "EVAL $" << _to_latex_() << "$" << endl;
        const auto res = term().eval(bn).margSumOut(Set({&bn.variable(var())}));
        if(this->getVerbosity()) cout << "END OF EVAL $" << _to_latex_() << "$ : " << res << endl;
        return res;
    }

    template<typename GUM_SCALAR>
    void ASTsum<GUM_SCALAR>::_print_(std::ostream& outs, int indent) const{
        auto L = std::vector<std::string>();
        const ASTtree<GUM_SCALAR>* cur = this;
        while(cur->type() == this->type()){
            auto ccur = static_cast<const ASTsum<GUM_SCALAR>*>(cur);
            L.push_back(ccur->var());
            cur = &ccur->term();
        }
        using namespace std;
        for(int i=0; i<indent; i++) outs << this->_continueNextLine_;
        outs << "sum on ";
        ___sort_comma_push_stream(outs, L);
        outs << " for" << endl;
        cur->_print_(outs, indent+1);
    }

    template<typename GUM_SCALAR>
    std::string ASTsum<GUM_SCALAR>::_to_latex_(NameCounter* nameOccur) const{
        auto L = std::vector<std::string>();
        const ASTtree<GUM_SCALAR>* cur = this;
        while(cur->type() == this->type()){
            auto ccur = static_cast<const ASTsum<GUM_SCALAR>*>(cur);
            L.push_back(ccur->var());
            cur = &ccur->term();
        }
        using namespace std;
        auto ss = stringstream();
        ss << "\\sum_{";
        ___sort_comma_push_stream(ss, this->_latext_var_present_(L, nameOccur));
        ss << "}{";
        cur->_print_(ss, 0);
        ss << "}";

        for(const auto& v : L) (*nameOccur)[v] -= 1;

        return ss.str();
    }

    template<typename GUM_SCALAR>
    std::string ASTsum<GUM_SCALAR>::_to_latex_indep_(NameCounter* nameOccur) const{
        return "\\left(" + _to_latex_(nameOccur) + "\\right)";
    }

    template<typename GUM_SCALAR, std::forward_iterator Iter, std::sentinel_for<Iter> Sen>
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTreesI(Iter begin, Sen end){
        if(begin+1 == end) return std::move(*begin);
        return std::make_unique<ASTmult<GUM_SCALAR>>(std::move(*begin), std::move(productOfTreesI<GUM_SCALAR, Iter>(begin+1, end)));
    }

    // INLINE templates

    template<typename GUM_SCALAR>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::vector<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs){
        auto ret = productOfTreesI<GUM_SCALAR, decltype(xs.begin())>(xs.begin(), xs.end());
        xs.clear();
        return std::move(ret);
    }

    template<typename GUM_SCALAR>
    INLINE
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Set<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs){
        auto ret = productOfTreesI<GUM_SCALAR, decltype(xs.begin())>(xs.begin(), xs.end());
        xs.clear();
        return std::move(ret);
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