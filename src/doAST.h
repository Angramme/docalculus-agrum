
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
        friend std::ostream& operator<< ( std::ostream& outs, const ASTtree<GUM_SCALAR>& p );
        // template<typename>
        // friend class ASTBinaryOp<GUM_SCALAR>;

    private:
        std::string _type_;
    protected:
        static bool _verbose_;
        static std::string _continueNextLine_;

    public:
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
        std::string toLatex(const NameCounter& nameOccur) const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const = 0;

    protected:
        virtual void _print_(std::ostream& outs, int indent) const = 0;

        /**
         * @brief internal conversion to latex, internal precedence rules can 
         * break it if it is inserted into another expression
         * 
         * @param nameOccur 
         * @return std::string 
         */
        virtual std::string _to_latex_(const NameCounter& nameOccur) const = 0;

        /**
         * @brief internal conversion to latex, resistant to internal precedcedence 
         * breaking it
         * 
         * @param nameOccur 
         * @return std::string 
         */
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const = 0;

        /**
         * @brief Change the latex representation of a variable w.r.t the number of 
         * occurrences of this variable : for instance, add primes when necessary
         * 
         * @param src the name
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented name
         */
        constexpr static std::string _latext_var_present_(const std::string& src, const NameCounter& nameOccur); 
        
        /**
         * @brief Change the latex representation of variables w.r.t the number of 
         * occurrences of these variables : for instance, add primes when necessary
         * 
         * @param src the names
         * @param nameOccur the dict that gives the number of occurrences for 
         * each variable (default value 0 if the variable is not a key in this dict)
         * @return constexpr std::string the presented names
         */
        constexpr static std::vector<std::string> _latext_var_present_(const std::vector<std::string>& src, const NameCounter& nameOccur); 
    };   // TODO : finish

    /**
     * @brief NOT IMPLEMENTED! stringify a CausalFormula tree
     * 
     * @param outs 
     * @param p 
     * @return std::ostream& 
     */
    template<typename GUM_SCALAR>
    std::ostream& operator<< ( std::ostream& outs, const ASTtree<GUM_SCALAR>& p );


    /**
     * @brief Represents a generic binary node for the CausalFormula. 
     * The op1 and op2 are the two operands of the class.
     * 
     */
    template<typename GUM_SCALAR>
    class ASTBinaryOp : public ASTtree<GUM_SCALAR> {
    private:
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

    protected:
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
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;
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
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;
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
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;
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
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;
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
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual void _print_(std::ostream& outs, int indent) const;
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;
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
        const NameSet& varNames() const;

        /**
         * @brief Evaluation of a AST tree from inside a BN
         * 
         * @param bn the observational Bayesian network in which will be done the computations
         * @return const Potential<GUM_SCALAR>&  the resulting Potential
         */
        virtual const Potential<GUM_SCALAR>& eval(const BayesNet<GUM_SCALAR>& bn) const override;

    protected:
        virtual void _print_(std::ostream& outs, int indent) const override;
        virtual std::string _to_latex_(const NameCounter& nameOccur) const override;
        virtual std::string _to_latex_indep_(const NameCounter& nameOccur) const override;    
    };

//   def copy(self) -> "ASTtree":
//     return ASTjointProba(self.varNames)



// class ASTsum(ASTtree):
//   """
//   Represents a sum over a variable of a :class:`causal.ASTtree`.

//   Parameters
//   ----------
//   var: str
//     name of the variable on which to sum
//   term: ASTtree
//     the tree to be evaluated
//   """

//   def __init__(self, var: str, term: ASTtree):
//     """
//     Represents a sum over a variable of a :class:`causal.ASTtree`.

//     Parameters
//     ----------
//     var: str
//       name of the variable on which to sum
//     term: ASTtree
//       the tree to be evaluated
//     """
//     super().__init__("_sum_")

//     va = var if isinstance(var, list) else [var]
//     self.var = va[0]

//     if len(va) > 1:
//       self._term = ASTsum(va[1:], term)
//     else:
//       self._term = term

//   @property
//   def term(self) -> ASTtree:
//     """
//     Returns
//     -------
//     ASTtree
//       the term to sum
//     """
//     return self._term

//   def __str__(self, prefix: str = "") -> str:
//     l = []
//     a = self
//     while a.type == "_sum_":
//       l.append(a.var)
//       a = a.term
//     return f'{prefix}sum on {",".join(sorted(l))} for\n{a.__str__(prefix + self._continueNextLine)}'

//   def copy(self) -> "ASTtree":
//     return ASTsum(self.var, self.term.copy())

//   def protectToLatex(self, nameOccur: Dict[str, int]) -> str:
//     return "\\left(" + self.fastToLatex(nameOccur) + "\\right)"

//   def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
//     la = []
//     a = self
//     while a.type == "_sum_":
//       la.append(a.var)
//       nameOccur[a.var] += 1
//       a = a.term

//     res = "\\sum_{" + (",".join(self._latexCorrect(la, nameOccur))) + "}{" + a.fastToLatex(nameOccur) + "}"
//     for v in la:
//       nameOccur[v] -= 1

//     return res

//   def eval(self, contextual_bn: "pyAgrum.BayesNet") -> "pyAgrum.Potential":
//     if self._verbose:
//       print(f"EVAL ${self.fastToLatex(defaultdict(int))}$", flush=True)

//     res = self.term.eval(contextual_bn).margSumOut([self.var])

//     if self._verbose:
//       print(f"END OF EVAL ${self.fastToLatex(defaultdict(int))}$ : {res}", flush=True)

//     return res


// def productOfTrees(lterms: List[ASTtree]) -> ASTtree:
//   """
//   create an ASTtree for a sequence of multiplications of ASTtree

//   Parameters
//   ----------
//   lterms: List[ASTtree]
//     the trees (as ASTtree) to multiply

//   Returns
//   -------
//   ASTtree
//     the ASTtree representing the tree of multiplications

//   """
//   if len(lterms) == 1:
//     return lterms[0]
//   return ASTmult(lterms[0], productOfTrees(lterms[1:]))

//   def fastToLatex(self, nameOccur: Dict[str, int]) -> str:
//     """
//     Internal virtual function to create a LaTeX representation of the ASTtree

//     Parameters
//     ----------
//     nameOccur: Dict[str,int]
//       the number of occurrence for each variable

//     Returns
//     -------
//     str
//       LaTeX representation of the tree
//     """
//     raise NotImplementedError

}


#include "doAST_tpl.h"
#ifndef GUM_NO_INLINE
#  include "doAST_inl.h"
#endif

#endif 