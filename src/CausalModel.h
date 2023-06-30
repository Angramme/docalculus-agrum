


#ifndef GUM_CAUSAL_MODEL_H
#define GUM_CAUSAL_MODEL_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/graphicalModels/DAGmodel.h>
#include <doorCriteria.h>
#include <utility>
#include <string>
#include <optional>

namespace gum{

   /**
      * @class CausalModel
      * @brief Class representing a Causal Model.
      *
      * From an observational BNs and the description of latent variables, 
      * this class represent a complet causal model obtained by adding the 
      * latent variables specified in ``latentVarsDescriptor`` to the Bayesian
      * network ``bn``.
      * 
      * @param bn an observational Bayesian network
      * @param latentVarsDescriptor list of couples (<latent variable name>, 
      * <list of affected variables' ids>).
      * @param keepArcs By default, the arcs between variables affected by a 
      * common latent variable will be removed but this can be avoided by setting 
      * ``keepArcs`` to ``True``
      * @author Kacper Ozieblowski
      */
   template <typename GUM_SCALAR>
   class CausalModel : public DAGmodel {
   private:
      const gum::BayesNet<GUM_SCALAR>& _ob_BN_; ///< observational bayes net
      // self.__latentVarsDescriptor = latentVarsDescriptor
      bool _keepArcs_;
      gum::BayesNet<GUM_SCALAR> _ca_BN_; ///< causal bayes net
      gum::NodeSet _lat_ ;
      gum::HashTable<gum::NodeId, std::string> _names_;

   public: 
      CausalModel(const gum::BayesNet<GUM_SCALAR>& bn,
               const std::vector<std::pair<std::string, std::vector<gum::NodeId>>>& latentVarDescriptors = {},
               bool keepArcs = false
               );

      CausalModel(const CausalModel& ot);
    
      /**
       * @brief Add a new latent variable with a name, a pair of children and replacing (or not) correlations between children.
       * 
       * @param name the name of the latent variable
       * @param lchild the list of children
       * @param keepArcs do we keep (or not) the arc between the children ?
       */
      void addLatentVariable(const std::string& name, const std::vector<std::string>& lchild, bool keepArcs = false);

      /**
       * @brief Add a new latent variable with a name, a pair of children and replacing (or not) correlations between children.
       * 
       * @param name the name of the latent variable
       * @param lchild the list of children
       * @param keepArcs do we keep (or not) the arc between the children ?
       */
      void addLatentVariable(const std::string& name, const std::vector<gum::NodeId>& lchild, bool keepArcs = false);
      /**
       * @warning do not infer any computations in this model. It is strictly a structural model
       * @return const gum::BayesNet<GUM_SCALAR>& 
       */
      const gum::BayesNet<GUM_SCALAR>& causalBN() const;

      /**
       * @return const gum::BayesNet<GUM_SCALAR>& 
       */
      const gum::BayesNet<GUM_SCALAR>& observationalBN() const;

      /**
       * @param name child node name
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& parents(std::string name) const;

      /**
       * @param id child node id 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& parents(gum::NodeId id) const;

      /**
       * @param name parent node name 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& children(std::string name) const;

      /**
       * @param id parent node id 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& children(gum::NodeId id) const;

      /// returns the children of a set of nodes
      NodeSet children(const NodeSet& ids) const;
      NodeSet children(const std::vector< std::string >& names) const;

      /**
       * @brief Returns a mapping from node's id to it's corresponding name.
       * 
       * @return const decltype(_names_)& 
       */
      const decltype(CausalModel<GUM_SCALAR>::_names_)& names() const;

      /**
       * @param name 
       * @return gum::NodeId 
       */
      gum::NodeId idFromName(const std::string& name) const;

      /**
       * @brief Returns the set of ids of latent variables in the causal model.
       * 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& latentVariablesIds() const;

      /**
       * @brief Erase the arc a->b
       * 
       * @param a id fo the first node
       * @param b id fo the second node
       */
      void eraseCausalArc(gum::NodeId a, gum::NodeId b);

      /**
       * @brief Erase the arc a->b
       * 
       * @param a id fo the first node
       * @param b id fo the second node
       */
      void eraseCausalArc(const std::string& a, const std::string& b);

      /**
       * @brief Add the arc a->b
       * 
       * @param a
       * @param b 
       */
      void addCausalArc(gum::NodeId a, gum::NodeId b);

      /**
       * @brief Add the arc a->b
       * 
       * @param a
       * @param b 
       */
      void addCausalArc(const std::string& a, const std::string& b);

      /**
       * @brief Check if the arc a->b exists
       * 
       * @param a
       * @param b 
       */
      void existsArc(gum::NodeId a, gum::NodeId b);

      /**
       * @brief Check if the arc a->b exists
       * 
       * @param a
       * @param b 
       */
      void existsArc(const std::string& a, const std::string& b);

      /**
       * @brief Return the set of nodes.
       * 
       * @return const gum::NodeGraphPart& 
       */
      decltype(CausalModel<GUM_SCALAR>::_ca_BN_.nodes()) nodes() const ;

      /**
       * @brief Return the set of arcs.
       * 
       * @return const gum::ArcSet& 
       */
      const gum::ArcSet& arcs() const;

      /**
       * @brief Returns a map between variables and nodes of this causal gum::BayesNet.
       *
       * @return Returns a constant reference to the gum::VariableNodeMap.
       */
      const gum::VariableNodeMap& variableNodeMap() const; 

      /**
       * @brief Returns a gum::DiscreteVariable given its gum::NodeId in the
       *        causal gum::BayesNet.
       *
       * @param id The variable's id to return.
       * @returns Returns a constant reference of the gum::DiscreteVariable
       *          corresponding to id in the gum::BayesNet.
       * @throw NotFound Raised if id does not match a a variable in the
       *                 gum::BayesNet.
       */
      const gum::DiscreteVariable& variable(gum::NodeId x) const;

      /**
       * @brief Returns a variable's id in the causal gum::BayesNet.
       *
       * @param var The variable from which the gum::NodeId is returned.
       * @return Returns the gum::DiscreteVariable gum::NodeId in the
       *         gum::BayesNet.
       * @throw NotFound If var is not in the gum::BayesNet.
       */
      gum::NodeId nodeId(const gum::DiscreteVariable& x) const;

      /**
       * @brief Returns a variable given its name in the causal gum::BayesNet.
       *
       * @param name The variable's name in the gum::BayesNet.
       * @return Returns the gum::DiscreteVariable named name in the
       * gum::BayesNet.
       * @throw NotFound Raised if name does not match a variable in the
       * gum::BayesNet.
       */
      const gum::DiscreteVariable& variableFromName(const std::string& name) const;

      /**
       * @brief Check if a backdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found backdoor. Otherwise return the found backdoors as set of ids.
       */
      std::optional<gum::NodeSet> backDoor(std::string cause, std::string effect);

      /**
       * @brief Check if a backdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found backdoor. Otherwise return the found backdoors as set of ids.
       */
      std::optional<gum::NodeSet> backDoor(gum::NodeId cause, gum::NodeId effect);

      /**
       * @brief Check if a backdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found backdoor. Otherwise return the found backdoors as set of names.
       */
      std::optional<std::set<std::string>> backDoor_withNames(std::string cause, std::string effect);

      /**
       * @brief Check if a backdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found backdoor. Otherwise return the found backdoors as set of names.
       */
      std::optional<std::set<std::string>> backDoor_withNames(gum::NodeId cause, gum::NodeId effect);


      /**
       * @brief Check if a frontdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found frontdoor. Otherwise return the found frontdoors as set of ids.
       */
      std::optional<gum::NodeSet> frontDoor(std::string cause, std::string effect);

      /**
       * @brief Check if a frontdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found frontdoor. Otherwise return the found frontdoors as set of ids.
       */
      std::optional<gum::NodeSet> frontDoor(gum::NodeId cause, gum::NodeId effect);

      /**
       * @brief Check if a frontdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found frontdoor. Otherwise return the found frontdoors as set of names.
       */
      std::optional<std::set<std::string>> frontDoor_withNames(std::string cause, std::string effect);

      /**
       * @brief Check if a frontdoor exists between `cause` and `effect`
       *
       * @param cause int|str : the nodeId or the name of the cause
       * @param effect int|str : the nodeId or the name of the effect
       * @return nullopt if not found frontdoor. Otherwise return the found frontdoors as set of names.
       */
      std::optional<std::set<std::string>> frontDoor_withNames(gum::NodeId cause, gum::NodeId effect);
   
      /// @name Variable manipulation methods.
      /// @{
      /**
       * Returns a constant reference to the dag of this Bayes Net.
       */
      const DAG& dag() const;

      /**
       * Returns the number of variables in this Directed Graphical Model.
       */
      virtual Size size() const final;

      /**
       * Returns the number of arcs in this Directed Graphical Model.
       */
      Size sizeArcs() const;

      /**
       * Return true if this node exists in this graphical model.
       */
      bool exists(NodeId node) const final;
      bool exists(const std::string& name) const final;

      /// returns the parents of a node and the node
      /**
       * @param id the node which is the head of an arc with the returned nodes
       * @param name the name of the node the node which is the head of an arc with
       * the returned nodes*/
      NodeSet family(const NodeId id) const final;
      NodeSet family(const std::string& name) const final;

      /// returns the set of nodes with directed path outgoing from a given node
      /** Note that the set of nodes returned may be empty if no path within the
       * ArcGraphPart is outgoing from the given node.
       * @param id the node which is the tail of a directed path with the returned
       * nodes
       * @param name the name of the node which is the tail of a directed path with
       * the returned nodes */
      NodeSet descendants(const NodeId id) const;
      NodeSet descendants(const std::string& name) const;

      /// returns the set of nodes with directed path ingoing to a given node
      /** Note that the set of nodes returned may be empty if no path within the
       * ArcGraphPart is ingoing to the given node.
       * @param id the node which is the head of a directed path with the returned
       * nodes
       * @param name the name of the node which is the head of a directed path with
       * the returned nodes */
      NodeSet ancestors(const NodeId id) const;
      NodeSet ancestors(const std::string& name) const;

      /** build a UndiGraph by moralizing the Ancestral Graph of a set of Nodes
       *
       * @param nodes the set of nodeId
       * @param nodenames the vector of names of nodes
       * @return the moralized ancestral graph
       */
      UndiGraph moralizedAncestralGraph(const NodeSet& nodes) const;
      UndiGraph moralizedAncestralGraph(const std::vector< std::string >& nodenames) const;
   
   
      /** check if node X and node Y are independent given nodes Z
       */
      bool isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const final;

      /** check if nodes X and nodes Y are independent given nodes Z
       */
      bool isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const final;

      bool isIndependent(const std::string&                Xname,
                        const std::string&                Yname,
                        const std::vector< std::string >& Znames) const ;

      bool isIndependent(const std::vector< std::string >& Xnames,
                        const std::vector< std::string >& Ynames,
                        const std::vector< std::string >& Znames) const;

      /**
       * The node's id are coherent with the variables and nodes of the topology.
       */
      UndiGraph moralGraph() const;

      /**
       * The topological order stays the same as long as no variable or arcs are
       * added or erased src the topology.
       * @param clear If false returns the previously created topology.
       */
      Sequence< NodeId > topologicalOrder() const;

      /// @return true if all the named node are the same and all the named arcs are
      /// the same
      bool hasSameStructure(const DAGmodel& other);

      protected:
      /**
       * Private copy operator.
       */
      DAGmodel& operator=(const DAGmodel& source);
   };

   /**
     * @brief Create an causal model induced by a subset of nodes.
     *
     * @param cm CausalModel : the causal model
     * @param sns Set[int] : the set of nodes
     * @return CausalModel the induced sub-causal model
     */
   template<typename GUM_SCALAR>
   CausalModel<GUM_SCALAR> inducedCausalSubModel(const CausalModel<GUM_SCALAR>& cm, const NodeSet& sns); 

   /**
     * @brief Create an causal model induced by a subset of nodes.
     *
     * @param cm CausalModel : the causal model
     * @return CausalModel the induced sub-causal model
     */
   template<typename GUM_SCALAR>
   CausalModel<GUM_SCALAR> inducedCausalSubModel(const CausalModel<GUM_SCALAR>& cm);
}

#include "CausalModel_tpl.h"
#ifndef GUM_NO_INLINE
#include "CausalModel_inl.h"
#endif

#endif 