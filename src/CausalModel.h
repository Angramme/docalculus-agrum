


#ifndef GUM_CAUSAL_MODEL_H
#define GUM_CAUSAL_MODEL_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/graphicalModels/DAGmodel.h>
#include <utility>
#include <string>

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

      // CausalModel(const CausalModel& ot)
      //    : CausalModel(gum::BayesNet<GUM_SCALAR>(ot._ob_BN_), )
      // {}
      //   def clone(self) -> "pyAgrum.causal.CausalModel":
   //     """
   //     Copy a causal model

   //     :return: the copy
   //     """
   //     return CausalModel(pyAgrum.BayesNet(self.__observationalBN),
   //                        self.__latentVarsDescriptor,
   //                        self.__keepArcs)

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
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& nodes() const ;

      /**
       * @brief Return the set of arcs.
       * 
       * @return const gum::ArcSet& 
       */
      const gum::ArcSet& arcs() const;

      /**
       * @brief Create a dot representation of the causal model.
       * 
       * @return std::string 
       */
      std::string toDot();
      //   def toDot(self) -> str:
   //     """
   //     Create a dot representation of the causal model

   //     :return: the dot representation in a string
   //     """
   //     res = "digraph {"

   //     # latent variables
   //     if pyAgrum.config['causal', 'show_latent_names'] == 'True':
   //       shap = "ellipse"
   //     else:
   //       shap = "point"
   //     res += f'''
   //     node [fillcolor="{pyAgrum.config['causal', 'default_node_bgcolor']}",
   //           fontcolor="{pyAgrum.config['causal', 'default_node_fgcolor']}",
   //           style=filled,shape={shap}];
   //       '''
   //     res += "\n"

   //     for n in self.nodes():
   //       if n in self.latentVariablesIds():
   //         res += '   "' + self.names()[n] + '";' + "\n"
   //     # not latent variables
   //     res += f'''
   //     node [fillcolor="{pyAgrum.config['causal', 'default_node_bgcolor']}",
   //           fontcolor="{pyAgrum.config['causal', 'default_node_fgcolor']}",
   //           style=filled,shape="ellipse"];
   //       '''
   //     res += "\n"

   //     for n in self.nodes():
   //       if n not in self.latentVariablesIds():
   //         res += '   "' + self.names()[n] + '";' + "\n"

   //     for a, b in self.arcs():
   //       res += '   "' + self.names()[a] + '"->"' + self.names()[b] + '" '
   //       if a in self.latentVariablesIds() or b in self.latentVariablesIds():
   //         res += ' [style="dashed"];'
   //       else:
   //         black_color = pyAgrum.config['notebook', 'default_arc_color']
   //         res += ' [color="' + black_color + ':' + black_color + '"];'
   //       res += "\n"

   //     res += "\n};"
   //     return res


      // const gum::NodeSet& backDoor(gum::NodeId cause, gum::NodeId effect){
      //    for bd in backdoor_generator(self, icause, ieffect, self.latentVariablesIds()):
      //       return bd;
      //    return None
      // }
   };

   //   def backDoor(self, cause: Union[NodeId, str], effect: Union[NodeId, str], withNames: bool = True) -> Union[
   //     None, NameSet, NodeSet]:
   //     """
   //     Check if a backdoor exists between `cause` and `effect`

   //     Parameters
   //     ----------
   //     cause: int|str
   //       the nodeId or the name of the cause
   //     effect: int|str
   //       the nodeId or the name of the effect
   //     withNames: bool
   //       wether we use ids (int) or names (str)

   //     Returns
   //     -------
   //     None|Set[str]|Set[int]
   //       None if no found backdoor. Otherwise return the found backdoors as set of ids or set of names.
   //     """
   //     icause = self.__observationalBN.idFromName(cause) if isinstance(cause, str) else cause
   //     ieffect = self.__observationalBN.idFromName(effect) if isinstance(effect, str) else effect

   //     for bd in backdoor_generator(self, icause, ieffect, self.latentVariablesIds()):
   //       if withNames:
   //         return {self.__observationalBN.variable(i).name() for i in bd}

   //       return bd

   //     return None

   //   def frontDoor(self, cause: Union[NodeId, str], effect: Union[NodeId, str], withNames: bool = True) -> Union[
   //     None, NameSet, NodeSet]:
   //     """
   //     Check if a frontdoor exists between cause and effet

   //     Parameters
   //     ----------
   //     cause: int|str
   //       the nodeId or the name of the cause
   //     effect: int|str
   //       the nodeId or the name of the effect
   //     withNames: bool
   //       wether we use ids (int) or names (str)

   //     Returns
   //     -------
   //     None|Set[str]|Set[int]
   //       None if no found frontdoot. Otherwise return the found frontdoors as set of ids or set of names.
   //     """
   //     icause = self.__observationalBN.idFromName(cause) if isinstance(cause, str) else cause
   //     ieffect = self.__observationalBN.idFromName(effect) if isinstance(effect, str) else effect

   //     for fd in frontdoor_generator(self, icause, ieffect, self.latentVariablesIds()):
   //       if withNames:
   //         return {self.__observationalBN.variable(i).name() for i in fd}

   //       return fd

   //     return None


   // def inducedCausalSubModel(cm: CausalModel, sns: NodeSet = None) -> CausalModel:
   //   """
   //   Create an causal model induced by a subset of nodes.

   //   Parameters
   //   ----------
   //   cm: CausalModel
   //     the causal model
   //   sns: Set[int]
   //     the set of nodes

   //   Returns
   //   -------
   //   CausalModel
   //     the induced sub-causal model
   //   """
   //   if sns is None:
   //     sns = cm.nodes()
   //   nodes = sns - cm.latentVariablesIds()

   //   bn = pyAgrum.BayesNet()

   //   for n in nodes:
   //     bn.add(cm.observationalBN().variable(n), n)

   //   for x, y in cm.arcs():
   //     if y in nodes:
   //       if x in nodes:
   //         bn.addArc(x, y)

   //   names = cm.names()
   //   latentVarsDescriptor = []
   //   lats = cm.latentVariablesIds()
   //   for latentVar in lats:
   //     inters = cm.children(latentVar) & nodes
   //     if len(inters) > 0:
   //       latentVarsDescriptor.append((names[latentVar],
   //                                    list(inters)))

   //   return CausalModel(bn, latentVarsDescriptor, True)
}

#include "CausalModel_tpl.h"
#endif 