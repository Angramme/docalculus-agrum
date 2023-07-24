
#ifndef DO_AST_H
#define DO_AST_H

#include <string>
#include <memory>
#include <iterator>

#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/BN/BayesNet.h>


namespace gum{

    typedef Set<std::string> NameSet;
    typedef HashTable<std::string, size_t> NameCounter;

    // I know, I know, a bit ugly, but this was by far the most elegant solution compared to CRTP clone semantics 
    // or even copy-pasting the whole function by hand everywhere
    #define ___ASTtree_clone_function_injector_MACRO___ \
        virtual std::unique_ptr<ASTtree<GUM_SCALAR>> clone() const override;
    
    /**
     * @brief Represents a generic node for the CausalFormula. The type of the node will be registered in a string.
     * 
     */
    template<typename GUM_SCALAR>
    class ASTtree{
    protected:
        static bool _verbose_;
        static std::string _continueNextLine_;

    public:
        typedef GUM_SCALAR GUM_SCALAR;
        /**
         * @brief Represents a generic node for the CausalFormula. The type of the node will be registered in a string.
         * 
         * @param verbose if True, add some messages
         */
        ASTtree();
        ASTtree(const ASTtree<GUM_SCALAR>& other);
        ~ASTtree();

        /**
         * @brief Set the Verbosity of messages.
         * If true some additional messages will appear.
         * By default false.
         * 
         * @param value 
         */
        static void setVerbosity(bool value); 

        /**
         * @brief Get the Verbosity level
         * 
         * @return true some additional messages will appear.
         * @return false default value
         */
        static bool getVerbosity();

        /**
         * @brief Set the newline prefix used inside _print_ function.
         * 
         * @param val 
         */
        static void setContinueNextLine(const std::string& val);
        
        /**
         * @brief Get the newline prefix used inside _print_ function.
         * 
         * @param val 
         */
        static const std::string& getContinueNextLine();

        /**
         * @brief type getter
         * 
         * @return const std::string& 
         */
        virtual std::string type() const = 0;

        /**
         * @brief Create a LaTeX representation from ASTtree
         * 
         * @param nameOccur the number of occurences of each variable
         * @return std::string the LaTeX representation pf the tree
         */
        INLINE std::string toLatex(NameCounter* nameOccur) const;

        /**
         * @brief Evaluation of an AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const = 0;

        virtual void _print_(std::ostream& outs, int indent) const = 0;

        /**
         * @brief internal conversion to latex, internal precedence rules can 
         * break it if it is inserted into another expression
         * 
         * @param nameOccur 
         * @return std::string 
         */
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const = 0;

        /**
         * @brief Performs a deep clone of the structure
         * 
         * @return std::unique_ptr<Base> 
         */
        virtual std::unique_ptr<ASTtree<GUM_SCALAR>> clone() const = 0;

    protected:
        /**
         * @brief internal conversion to latex, resistant to internal precedcedence 
         * breaking it
         * 
         * @param nameOccur 
         * @return std::string 
         */
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const = 0;

        /**
         * @brief Change the latex representation of a variable w.r.t the number of 
         * occurrences of this variable : for instance, add primes when necessary
         * 
         * @param src the name
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented name
         */
        constexpr static std::string _latext_var_present_(const std::string& src, NameCounter* nameOccur = nullptr); 
        
        /**
         * @brief Change the latex representation of variables w.r.t the number of 
         * occurrences of these variables : for instance, add primes when necessary
         * 
         * @param src the names
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented names
         */
        constexpr static std::vector<std::string> _latext_var_present_(const std::vector<std::string>& src, NameCounter* nameOccur = nullptr); 

        /**
         * @brief Change the latex representation of variables w.r.t the number of 
         * occurrences of these variables : for instance, add primes when necessary
         * 
         * @param src the names
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented names
         */
        constexpr static std::vector<std::string> _latext_var_present_(const Set<std::string>& src, NameCounter* nameOccur = nullptr); 
        
        /**
         * @brief Change the latex representation of variables w.r.t the number of 
         * occurrences of these variables : for instance, add primes when necessary
         * 
         * @param src the names
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented names
         */
        template<typename Iter>
        constexpr static std::vector<std::string> _latext_var_present_(Iter b_src, Iter e_src, NameCounter* nameOccur = nullptr); 
    };  


    /**
     * @brief Represents a generic binary node for the CausalFormula. 
     * The op1 and op2 are the two operands of the class.
     * 
     */
    template<typename GUM_SCALAR>
    class ASTBinaryOp : public ASTtree<GUM_SCALAR> {
    protected:
        std::unique_ptr<ASTtree<GUM_SCALAR>> _op1_;
        std::unique_ptr<ASTtree<GUM_SCALAR>> _op2_;
    public:
        /**
         * @brief Represents a generic binary node for the CausalFormula. 
         * Note: we take ownership of both op1 and op2
         * @param typ the type of the node (will be specified in concrete children classes.
         * @param op1 left operand
         * @param op2 right operand
         * @param verbose if True, add some messages
         */
        ASTBinaryOp(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);
        ASTBinaryOp(const ASTBinaryOp<GUM_SCALAR>& other);
        ~ASTBinaryOp();
        ASTBinaryOp() = delete;

        /**
         * @brief get left operand
         * 
         * @return const ASTtree& 
         */
        INLINE const ASTtree<GUM_SCALAR>& op1() const;
        
        /**
         * @brief get right operand
         * 
         * @return const ASTtree& 
         */
        INLINE const ASTtree<GUM_SCALAR>& op2() const;
        
        /**
         * @brief get left operand
         * 
         * @return const ASTtree& 
         */
        INLINE ASTtree<GUM_SCALAR>& op1();

        /**
         * @brief get right operand
         * 
         * @return const ASTtree& 
         */
        INLINE ASTtree<GUM_SCALAR>& op2();

        virtual void _print_(std::ostream& outs, int indent) const;
    };

    /**
     * @brief Represents the sum of 2 ASTtree instances
     * 
     */
    template<typename GUM_SCALAR>
    class ASTplus : public ASTBinaryOp<GUM_SCALAR> {
    public:
        /**
         * @brief Represents the sum of 2 ASTtree instances
         * 
         * @param op1 left operand
         * @param op2 right operand
         */
        ASTplus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);
        ASTplus() = delete;
        ASTplus(const ASTplus<GUM_SCALAR>& other);
        ~ASTplus();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };


    /**
     * @brief Represents the substraction of 2 ASTtree instances
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    class ASTminus : public ASTBinaryOp<GUM_SCALAR> {
    public: 
        /**
         * @brief Represents the substraction of 2 ASTtree instances
         * 
         * @param op1 left operand
         * @param op2 right operand
         */
        ASTminus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);
        ASTminus() = delete;
        ASTminus(const ASTminus<GUM_SCALAR>& other);
        ~ASTminus();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };


    /**
     * @brief Represents the multiplication of 2 ASTtree instances
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    // class ASTmult : public ClonableCRTP<ASTmult<GUM_SCALAR>, ASTBinaryOp<GUM_SCALAR>> {
    class ASTmult : public ASTBinaryOp<GUM_SCALAR> {
    public: 
        /**
         * @brief Represents the multiplication of 2 ASTtree instances
         * 
         * @param op1 left operand
         * @param op2 right operand
         */
        ASTmult(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);
        ASTmult() = delete;
        ASTmult(const ASTmult<GUM_SCALAR>& other);
        ~ASTmult();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    /**
     * @brief Represents the division of 2 ASTtree instances 
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    class ASTdiv : public ASTBinaryOp<GUM_SCALAR> {
    public: 
        /**
         * @brief Represents the division of 2 ASTtree instances
         * 
         * @param op1 left operand
         * @param op2 right operand
         */
        ASTdiv(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);
        ASTdiv() = delete;
        ASTdiv(const ASTdiv<GUM_SCALAR>& other);
        ~ASTdiv();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };


    /**
     * @brief Represents a conditional probability \f( P_{bn}(vars|knw) \f) that can be computed by an inference in a BN.
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    class ASTPosteriorProba : public ASTtree<GUM_SCALAR> {
    private:
        const std::shared_ptr<BayesNet<GUM_SCALAR>> _bn_; //< observational BN
        const std::shared_ptr<NameSet> _varset_; //< a set of variable names (in the BN) conditioned in the posterior
        const std::shared_ptr<NameSet> _knwset_; //< a set of variable names (in the BN) conditioning in the posterior

    public:
        /**
         * @brief Represents a conditional probability \f( P_{bn}(vars|knw) \f) that can be computed by an inference in a BN.
         * 
         * @param bn observational BN
         * @param varset a set of variable names (in the BN) conditioned in the posterior
         * @param knwset a set of variable names (in the BN) conditioning in the posterior
         */
        ASTPosteriorProba(const std::shared_ptr<BayesNet<GUM_SCALAR>> bn, const std::shared_ptr<NameSet> varset, const std::shared_ptr<NameSet> knwset);
        ASTPosteriorProba() = delete;
        ASTPosteriorProba(const ASTPosteriorProba<GUM_SCALAR>& other);
        ~ASTPosteriorProba();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief (Conditioned) vars in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        INLINE const NameSet& vars() const;

        /**
         * @brief (Conditioning) knw in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        INLINE const NameSet& knw() const;

        /**
         * @brief the observationnal BayesNet in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        INLINE const NameSet& bn() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

        virtual void _print_(std::ostream& outs, int indent) const;
    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    /**
     * @brief Represents a joint probability in the base observational part of the CausalModel
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    class ASTJointProba : public ASTtree<GUM_SCALAR> {
    private:
        const std::shared_ptr<NameSet> _varnames_;

    public:
        /**
         * @brief Represents a joint probability in the base observational part of the CausalModel
         * 
         * @param varNames a set of variable names
         */
        ASTJointProba(const std::shared_ptr<NameSet> varNames);
        ASTJointProba() = delete;
        ASTJointProba(const ASTJointProba<GUM_SCALAR>& other);
        ~ASTJointProba();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;

        /**
         * @brief returns the set of names of car
         * 
         * @return const NameSet& 
         */
        INLINE const NameSet& vars() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

        virtual void _print_(std::ostream& outs, int indent) const override;
    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;    
    };


    /**
     * @brief Represents a sum over a variable of a :class:`causal.ASTtree`.
     * 
     */
    template<typename GUM_SCALAR>
    class ASTsum : public ASTtree<GUM_SCALAR> {
    private:
        const std::string _var_;
        const std::unique_ptr<ASTtree<GUM_SCALAR>> _term_;
    public:

        ASTsum(const ASTsum<GUM_SCALAR>& other);
        ASTsum() = delete;
        ~ASTsum();

        /// override of @ref ASTtree::type
        virtual std::string type() const override;
    
        /**
         * @brief Represents a sum over a variable of an ASTtree
         * 
         * @param var name of the variable on which to sum
         * @param term the tree to be evaluated
         */
        ASTsum(const std::string& var, const std::unique_ptr<ASTtree<GUM_SCALAR>> term);

        /**
         * @brief Represents a sum over variables of an ASTtree
         * 
         * @param var name of the variables on which to sum
         * @param term the tree to be evaluated
         */                
        ASTsum(std::initializer_list<std::string> var, std::unique_ptr<ASTtree<GUM_SCALAR>> term);

        /**
         * @brief Represents a sum over variables of an ASTtree
         * 
         * @param var name of the variables on which to sum
         * @param term the tree to be evaluated
         */                
        ASTsum(const std::vector<std::string>& var, std::unique_ptr<ASTtree<GUM_SCALAR>> term);

        /**
         * @brief Represents a sum over variables of an ASTtree
         * 
         * @tparam Iter 
         * @tparam Sen
         * 
         * @param var name of the variables on which to sum
         * @param term the tree to be evaluated
         */
        template<std::input_iterator Iter, std::sentinel_for<Iter> Sen>
        ASTsum(Iter b_var, Sen e_var, std::unique_ptr<ASTtree<GUM_SCALAR>> term);

        /**
         * @brief get the term to sum
         * 
         * @return const ASTtree<GUM_SCALAR>& 
         */
        INLINE const ASTtree<GUM_SCALAR>& term() const;
        
        /**
         * @brief get the var
         * 
         * @return const ASTtree<GUM_SCALAR>& 
         */
        INLINE const std::string& var() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        ___ASTtree_clone_function_injector_MACRO___

        virtual void _print_(std::ostream& outs, int indent) const override;
    protected: 
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;    
    };

    // no longer needed
    #undef ___ASTtree_clone_function_injector_MACRO___


    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @tparam Iter iterator of item type std::unique_ptr<gum::ASTtree<GUM_SCALAR>>
     * @param begin of the trees (as unique_ptr<ASTtree>) to multiply
     * @param end of the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR, std::forward_iterator Iter, std::sentinel_for<Iter> Sen>
        // requires std::same_as<std::iter_value_t<Iter>, std::unique_ptr<ASTtree<GUM_SCALAR>>>
    INLINE std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTreesI(Iter begin, Sen end);

    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @tparam Iterable iterable implementing begin() and end() of item type std::unique_ptr<gum::ASTtree<GUM_SCALAR>>
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR, typename Iterable>
    INLINE std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Iterable& v);
    
    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR>
    INLINE std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::initializer_list<std::unique_ptr<ASTtree<GUM_SCALAR>>> xs);
    
    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree.
     * Variant created purely so that user does not have to specify template parameters in 
     * the most common use cases (i.e vector and Set)
     * 
     * @tparam GUM_SCALAR 
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR>
    INLINE std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::vector<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs);
    
    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree.
     * Variant created purely so that user does not have to specify template parameters in 
     * the most common use cases (i.e vector and Set)
     * 
     * @tparam GUM_SCALAR 
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR>
    INLINE std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Set<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs);
}

/**
 * @brief stringify a CausalFormula tree
 * 
 * @param outs 
 * @param p 
 * @return std::ostream& 
 */
template<typename GUM_SCALAR>
std::ostream& operator<< ( std::ostream& outs, const gum::ASTtree<GUM_SCALAR>& p );


#include "doAST_tpl.h"
#ifndef GUM_NO_INLINE
#include "doAST_inl.h"
#endif

#endif 