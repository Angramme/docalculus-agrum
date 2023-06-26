
#ifndef DO_AST_H
#define DO_AST_H

#include <string>
#include <memory>

#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/BN/BayesNet.h>

namespace gum{

    typedef Set<std::string> NameSet;
    typedef HashTable<std::string, size_t> NameCounter;
    
    // template<typename GUM_SCALAR>
    // class ASTBinaryOp;

    /**
     * @brief Represents a generic node for the CausalFormula. The type of the node will be registered in a string.
     * 
     */
    template<typename GUM_SCALAR>
    class ASTtree{
        // friend std::ostream& operator<< ( std::ostream& outs, const ASTtree<GUM_SCALAR>& p );
        // template<typename>
        // friend class ASTBinaryOp<GUM_SCALAR>;

    private:
        std::string _type_;
    protected:
        static bool _verbose_;
        static std::string _continueNextLine_;

    public:
        typedef GUM_SCALAR GUM_SCALAR;
        /**
         * @brief Represents a generic node for the CausalFormula. The type of the node will be registered in a string.
         * 
         * @param typ the type of the node (will be specified in concrete children classes.
         * @param verbose if True, add some messages
         */
        ASTtree(const std::string& typ);

        //   def copy(self) -> "ASTtree":
        //     """
        //     Copy an CausalFormula tree

        //     Returns
        //     -------
        //     ASTtree
        //       the new causal tree
        //     """
        //     raise NotImplementedError

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
         * @brief type getter
         * 
         * @return const std::string& 
         */
        const std::string& type() const {
            return _type_;
        }

        /**
         * @brief Create a LaTeX representation from ASTtree
         * 
         * @param nameOccur the number of occurences of each variable
         * @return std::string the LaTeX representation pf the tree
         */
        std::string toLatex(NameCounter* nameOccur) const;

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
        // TODO: look at this template tho
        // template<std::input_iterator Iter, std::sentinel_for<Iter> Sen>
        template<typename Iter>
        constexpr static std::vector<std::string> _latext_var_present_i_(Iter b_src, Iter e_src, NameCounter* nameOccur = nullptr); 
    };   // TODO : finish


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
        ASTBinaryOp(const std::string& typ, std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

        /**
         * @brief get left operand
         * 
         * @return const ASTtree& 
         */
        const ASTtree<GUM_SCALAR>& op1() const;
        
        /**
         * @brief get right operand
         * 
         * @return const ASTtree& 
         */
        const ASTtree<GUM_SCALAR>& op2() const;
        
        /**
         * @brief get left operand
         * 
         * @return const ASTtree& 
         */
        ASTtree<GUM_SCALAR>& op1();

        /**
         * @brief get right operand
         * 
         * @return const ASTtree& 
         */
        ASTtree<GUM_SCALAR>& op2();

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

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    // TODO
    //   def copy(self) -> "ASTtree":
    //     return ASTplus(self.op1.copy(), self.op2.copy())

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

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    //   def copy(self) -> "ASTtree":
    //     return ASTminus(self.op1.copy(), self.op2.copy())


    /**
     * @brief Represents the multiplication of 2 ASTtree instances
     * 
     * @tparam GUM_SCALAR 
     */
    template<typename GUM_SCALAR>
    class ASTmult : public ASTBinaryOp<GUM_SCALAR> {
    public: 
        /**
         * @brief Represents the multiplication of 2 ASTtree instances
         * 
         * @param op1 left operand
         * @param op2 right operand
         */
        ASTmult(std::unique_ptr<ASTtree<GUM_SCALAR>> op1, std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    //   def copy(self) -> "ASTtree":
    //     return ASTmult(self.op1.copy(), self.op2.copy())

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

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

    //   def copy(self) -> "ASTtree":
    //     return ASTdiv(self.op1.copy(), self.copy(self.op2.copy()))


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

        /**
         * @brief (Conditioned) vars in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        const NameSet& vars() const;

        /**
         * @brief (Conditioning) knw in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        const NameSet& knw() const;

        /**
         * @brief the observationnal BayesNet in \f( P_{bn}(vars|knw) \f)
         * 
         * @return const Set<std::string>& 
         */
        const NameSet& bn() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        virtual void _print_(std::ostream& outs, int indent) const;
    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;
    };

//   def copy(self) -> "ASTtree":
//     return ASTposteriorProba(self.bn, self.vars, self.knw)

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

        /**
         * @brief returns the set of names of car
         * 
         * @return const NameSet& 
         */
        const NameSet& vars() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        virtual void _print_(std::ostream& outs, int indent) const override;
    protected:
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;    
    };

//   def copy(self) -> "ASTtree":
//     return ASTjointProba(self.varNames)

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
        const ASTtree<GUM_SCALAR>& term() const;
        
        /**
         * @brief get the var
         * 
         * @return const ASTtree<GUM_SCALAR>& 
         */
        const std::string& var() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual Potential<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& bn) const override;

        virtual void _print_(std::ostream& outs, int indent) const override;
    protected: 
        virtual std::string _to_latex_(NameCounter* nameOccur = nullptr) const override;
        virtual std::string _to_latex_indep_(NameCounter* nameOccur = nullptr) const override;    
    };

//   def copy(self) -> "ASTtree":
//     return ASTsum(self.var, self.term.copy())

    // /**
    //  * @brief create an ASTtree for a sequence of multiplications of ASTtree
    //  * 
    //  * @tparam GUM_SCALAR 
    //  * @param xs the trees (as unique_ptr<ASTtree>) to multiply
    //  * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
    //  */
    // template<typename GUM_SCALAR>
    // ASTtree<GUM_SCALAR> productOfTrees(std::vector<std::unique_ptr<gum::ASTtree<double>, std::default_delete<gum::ASTtree<double>>>, std::allocator<std::unique_ptr<gum::ASTtree<double>, std::default_delete<gum::ASTtree<double>>>>>& xs);
    
    // /**
    //  * @brief create an ASTtree for a sequence of multiplications of ASTtree
    //  * 
    //  * @tparam GUM_SCALAR 
    //  * @param xs the trees (as unique_ptr<ASTtree>) to multiply
    //  * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
    //  */
    // template<typename GUM_SCALAR>
    // ASTtree<GUM_SCALAR> productOfTrees(std::initializer_list<std::unique_ptr<ASTtree<GUM_SCALAR>>> xs);

    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @tparam Iter 
     * @tparam Sen
     * @param begin begin iterator to the trees (as unique_ptr<ASTtree>) to multiply
     * @param end end iterator to the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    // template<typename GUM_SCALAR, typename Iter>
    template<typename GUM_SCALAR, std::forward_iterator Iter, std::sentinel_for<Iter> Sen>
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTreesI(Iter begin, Sen end);
    
    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR>
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(std::vector<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs);
    
    /**
     * @brief create an ASTtree for a sequence of multiplications of ASTtree
     * 
     * @tparam GUM_SCALAR 
     * @param xs the trees (as unique_ptr<ASTtree>) to multiply
     * @return ASTtree<GUM_SCALAR>  the ASTtree representing the tree of multiplications
     */
    template<typename GUM_SCALAR>
    std::unique_ptr<ASTtree<GUM_SCALAR>> productOfTrees(Set<std::unique_ptr<ASTtree<GUM_SCALAR>>>& xs);
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
#  include "doAST_inl.h"
#endif

#endif 