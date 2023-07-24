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
    void ASTtree<GUM_SCALAR>::setContinueNextLine(const std::string& value){
        _continueNextLine_ = value;
    }

    template<typename GUM_SCALAR>
    const std::string& ASTtree<GUM_SCALAR>::getContinueNextLine() {
        return _continueNextLine_;
    }
    
    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR>::ASTtree() {
        GUM_CONSTRUCTOR(ASTtree)
    }

    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR>::ASTtree(const ASTtree<GUM_SCALAR>& other) {
        GUM_CONS_CPY(ASTtree);
    }

    template<typename GUM_SCALAR>
    ASTtree<GUM_SCALAR>::~ASTtree(){
        GUM_DESTRUCTOR(ASTtree);
    }


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
    constexpr std::vector<std::string> ASTtree<GUM_SCALAR>::_latext_var_present_(
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
    { return _latext_var_present_(src.cbegin(), src.cend(), nameOccur); }

    template<typename GUM_SCALAR>
    constexpr std::vector<std::string> ASTtree<GUM_SCALAR>::_latext_var_present_(
        const Set<std::string>& src, NameCounter* nameOccur)
    { return _latext_var_present_(src.cbegin(), src.cend(), nameOccur); }

    template<typename GUM_SCALAR>
    ASTBinaryOp<GUM_SCALAR>::ASTBinaryOp(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2)
        : _op1_(std::move(op1)), _op2_(std::move(op2))
    {
        GUM_CONSTRUCTOR(ASTBinaryOp)
    }

    template<typename GUM_SCALAR>
    ASTBinaryOp<GUM_SCALAR>::ASTBinaryOp(const ASTBinaryOp<GUM_SCALAR>& other)
        : ASTtree<GUM_SCALAR>(static_cast<const ASTtree<GUM_SCALAR>&>(other)), _op1_(std::move(other._op1_->clone())), _op2_(std::move(other._op2_->clone()))
    {
        GUM_CONS_CPY(ASTBinaryOp)
    }

    template<typename GUM_SCALAR>
    ASTBinaryOp<GUM_SCALAR>::~ASTBinaryOp(){
        GUM_DESTRUCTOR(ASTBinaryOp)
    }

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
        : ASTBinaryOp<GUM_SCALAR>(std::move(op1), std::move(op2))
    {
        GUM_CONSTRUCTOR(ASTplus);
    }

    template<typename GUM_SCALAR>
    ASTplus<GUM_SCALAR>::ASTplus(const ASTplus<GUM_SCALAR>& other) 
        : ASTBinaryOp<GUM_SCALAR>(other)
    {
        GUM_CONS_CPY(ASTplus);
    }

    template<typename GUM_SCALAR>
    ASTplus<GUM_SCALAR>::~ASTplus() {
        GUM_DESTRUCTOR(ASTplus);
    }

    template<typename GUM_SCALAR>
    std::string ASTplus<GUM_SCALAR>::type() const {
        return "+";
    }

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
        : ASTBinaryOp<GUM_SCALAR>(std::move(op1), std::move(op2))
    {
        GUM_CONSTRUCTOR(ASTminus);
    }

    template<typename GUM_SCALAR>
    ASTminus<GUM_SCALAR>::ASTminus(const ASTminus<GUM_SCALAR>& other) 
        : ASTBinaryOp<GUM_SCALAR>(other)
    {
        GUM_CONS_CPY(ASTminus);
    }

    template<typename GUM_SCALAR>
    ASTminus<GUM_SCALAR>::~ASTminus() {
        GUM_DESTRUCTOR(ASTminus);
    }

    template<typename GUM_SCALAR>
    std::string ASTminus<GUM_SCALAR>::type() const {
        return "-";
    }

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
        : ASTBinaryOp<GUM_SCALAR>(std::move(op1), std::move(op2))
    {
        GUM_CONSTRUCTOR(ASTmult);
    }

    template<typename GUM_SCALAR>
    ASTmult<GUM_SCALAR>::ASTmult(const ASTmult<GUM_SCALAR>& other) 
        : ASTBinaryOp<GUM_SCALAR>(other)
    {
        GUM_CONS_CPY(ASTmult);
    }

    template<typename GUM_SCALAR>
    ASTmult<GUM_SCALAR>::~ASTmult() {
        GUM_DESTRUCTOR(ASTmult);
    }

    template<typename GUM_SCALAR>
    std::string ASTmult<GUM_SCALAR>::type() const {
        return "*";
    }

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
        : ASTBinaryOp<GUM_SCALAR>(std::move(op1), std::move(op2))
    {
        GUM_CONSTRUCTOR(ASTdiv);
    }

    template<typename GUM_SCALAR>
    ASTdiv<GUM_SCALAR>::ASTdiv(const ASTdiv<GUM_SCALAR>& other) 
        : ASTBinaryOp<GUM_SCALAR>(other)
    {
        GUM_CONS_CPY(ASTdiv);
    }

    template<typename GUM_SCALAR>
    ASTdiv<GUM_SCALAR>::~ASTdiv() {
        GUM_DESTRUCTOR(ASTdiv);
    }

    template<typename GUM_SCALAR>
    std::string ASTdiv<GUM_SCALAR>::type() const {
        return "/";
    }

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
        : _bn_(bn), _varset_(varset), _knwset_(_ASTPosteriorProba_init_knwset(*bn, *varset, *knwset))
    {
        GUM_CONSTRUCTOR(ASTPosteriorProba);
    }

    template<typename GUM_SCALAR>
    ASTPosteriorProba<GUM_SCALAR>::ASTPosteriorProba(const ASTPosteriorProba<GUM_SCALAR>& other) 
        : ASTtree<GUM_SCALAR>(other), _bn_(other._bn_), _varset_(other._varset_), _knwset_(other._knwset_)
    {
        GUM_CONS_CPY(ASTPosteriorProba);
    }

    template<typename GUM_SCALAR>
    ASTPosteriorProba<GUM_SCALAR>::~ASTPosteriorProba() {
        GUM_DESTRUCTOR(ASTPosteriorProba);
    }

    template<typename GUM_SCALAR>
    std::string ASTPosteriorProba<GUM_SCALAR>::type() const {
        return "_posterior_";
    }

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

        if(this->getVerbosity()) cout << "END OF EVAL $" << _to_latex_({}) << "$ : " << p << std::endl;

        return p;
    } 

    template<typename GUM_SCALAR>
    ASTJointProba<GUM_SCALAR>::ASTJointProba(const std::shared_ptr<NameSet> varNames)
        : _varnames_(varNames) 
    {
        GUM_CONSTRUCTOR(ASTJointProba);
    }

    template<typename GUM_SCALAR>
    ASTJointProba<GUM_SCALAR>::ASTJointProba(const ASTJointProba<GUM_SCALAR>& other) 
        : ASTtree<GUM_SCALAR>(other), _varnames_(other._varnames_)
    {
        GUM_CONS_CPY(ASTJointProba);
    }

    template<typename GUM_SCALAR>
    ASTJointProba<GUM_SCALAR>::~ASTJointProba() {
        GUM_DESTRUCTOR(ASTJointProba);
    }

    template<typename GUM_SCALAR>
    std::string ASTJointProba<GUM_SCALAR>::type() const {
        return "_joint_";
    }

    template<typename GUM_SCALAR> // TODO: this function crashes...
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
        : ASTsum(var.begin(), var.end(), std::move(term))
    {}

    template<typename GUM_SCALAR>
    ASTsum<GUM_SCALAR>::ASTsum(const std::vector<std::string>& var, std::unique_ptr<ASTtree<GUM_SCALAR>> term)
        : ASTsum(var.begin(), var.end(), std::move(term))    
    {}


    template<typename GUM_SCALAR>
    template<std::input_iterator Iter, std::sentinel_for<Iter> Sen>
    ASTsum<GUM_SCALAR>::ASTsum(Iter b_var, Sen e_var, std::unique_ptr<ASTtree<GUM_SCALAR>> term)
        : _var_(*b_var), _term_(b_var+1==e_var ? std::move(term) : std::make_unique<ASTsum>(b_var+1, e_var, std::move(term)))
    {
        GUM_CONSTRUCTOR(ASTsum)
    }

    template<typename GUM_SCALAR>
    ASTsum<GUM_SCALAR>::ASTsum(const ASTsum<GUM_SCALAR>& other)
        : ASTtree<GUM_SCALAR>(static_cast<const ASTtree<GUM_SCALAR>&>(other)), _var_(other._var_), _term_(std::move(other._term_->clone()))
    {
        GUM_CONS_CPY(ASTsum);
    }

    template<typename GUM_SCALAR>
    ASTsum<GUM_SCALAR>::~ASTsum(){
        GUM_DESTRUCTOR(ASTsum);
    }

    template<typename GUM_SCALAR>
    std::string ASTsum<GUM_SCALAR>::type() const {
        return "_sum_";
    }

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

    // I know, I know, a bit ugly, but this was by far the most elegant solution compared to CRTP clone semantics 
    // or even copy-pasting the whole function by hand everywhere
    #define ___ASTtree_clone_function_injector_MACRO___(ASTtype) \
        template<typename GUM_SCALAR> \
        std::unique_ptr<ASTtree<GUM_SCALAR>> ASTtype<GUM_SCALAR>::clone() const { \
            return std::unique_ptr<ASTtree<GUM_SCALAR>>(new std::remove_const_t<std::remove_reference_t<decltype(*this)>>(*this)); \
        }

    ___ASTtree_clone_function_injector_MACRO___(ASTplus)
    ___ASTtree_clone_function_injector_MACRO___(ASTminus)
    ___ASTtree_clone_function_injector_MACRO___(ASTmult)
    ___ASTtree_clone_function_injector_MACRO___(ASTdiv)
    ___ASTtree_clone_function_injector_MACRO___(ASTPosteriorProba)
    ___ASTtree_clone_function_injector_MACRO___(ASTJointProba)
    ___ASTtree_clone_function_injector_MACRO___(ASTsum)

    #undef ___ASTtree_clone_function_injector_MACRO___

}