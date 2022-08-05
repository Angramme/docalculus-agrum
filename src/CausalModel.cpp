
#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
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
   class CausalModel{
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
               )
               : _ob_BN_(bn), _keepArcs_(keepArcs), _ca_BN_(), _lat_(), _names_()
      {
         // we have to redefine attributes since the bn 
         // may be augmented by latent variables

         // nodes and arcs of BN
         for(const auto& n : bn.nodes()) _ca_BN_.add(bn.variable(n), n);
         for(const auto& a : bn.arcs()) _ca_BN_.addArc(a.tail(), a.head());

         for(const auto& nid : _ca_BN_.nodes()){
            _names_.insert(nid, _ca_BN_.variable(nid).name());
         }

         for(const auto& p : latentVarDescriptors)
            addLatentVariable(p.first, p.second, keepArcs);
      }

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
      void addLatentVariable(const std::string& name, const std::vector<std::string>& lchild, bool keepArcs = false){
         std::vector<gum::NodeId> ids(lchild.size());
         for(size_t i = 0; i<lchild.size(); i++) ids[i] = _ob_BN_.idFromName(lchild[i]);
         addLatentVariable(name, ids, keepArcs);
      }

      /**
       * @brief Add a new latent variable with a name, a pair of children and replacing (or not) correlations between children.
       * 
       * @param name the name of the latent variable
       * @param lchild the list of children
       * @param keepArcs do we keep (or not) the arc between the children ?
       */
      void addLatentVariable(const std::string& name, const std::vector<gum::NodeId>& lchild, bool keepArcs = false){
         // simplest variable to add : only 2 modalities for latent variables
         const auto id_latent = _ca_BN_.add(name, 2);
         _lat_.insert(id_latent);
         _names_.insert(id_latent, name);
      
         for(const auto& item : lchild) addCausalArc(id_latent, item);

         if(keepArcs) return;
         for(size_t i=0; i<lchild.size(); i++){
            for(size_t j=i+1; j<lchild.size(); j++){
               if(_ca_BN_.parents(lchild[j]).contains(lchild[i])) 
                  eraseCausalArc(lchild[i], lchild[j]);
               else if(_ca_BN_.parents(lchild[i]).contains(lchild[j])) 
                  eraseCausalArc(lchild[j], lchild[i]);
            }
         }
      }

      /**
       * @warning do not infer any computations in this model. It is strictly a structural model
       * @return const gum::BayesNet<GUM_SCALAR>& 
       */
      const gum::BayesNet<GUM_SCALAR>& causalBN() const {
         return _ca_BN_;
      }

      /**
       * @return const gum::BayesNet<GUM_SCALAR>& 
       */
      const gum::BayesNet<GUM_SCALAR>& observationalBN() const {
         return _ob_BN_;
      }

      /**
       * @param name child node name
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& parents(std::string name) const {
         return parents(_ca_BN_.idFromName(name));
      }

      /**
       * @param id child node id 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& parents(gum::NodeId id) const {
         return _ca_BN_.parents(id);
      }

      /**
       * @param name parent node name 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& children(std::string name) const {
         return _ca_BN_.children(_ca_BN_.idFromName(name));
      }

      /**
       * @param id parent node id 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& children(gum::NodeId id) const {
         return _ca_BN_.children(id);
      }

      /**
       * @brief Returns a mapping from node's id to it's corresponding name.
       * 
       * @return const decltype(_names_)& 
       */
      const decltype(_names_)& names() const {
         return _names_;
      }

      /**
       * @param name 
       * @return gum::NodeId 
       */
      gum::NodeId idFromName(const std::string& name) const {
         return _ca_BN_.idFromName(name);
      }

      /**
       * @brief Returns the set of ids of latent variables in the causal model.
       * 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& latentVariablesIds() const {
         return _lat_;
      }

      /**
       * @brief Erase the arc a->b
       * 
       * @param a id fo the first node
       * @param b id fo the second node
       */
      void eraseCausalArc(gum::NodeId a, gum::NodeId b){
         _ca_BN_.eraseArc(a, b);
      }

      /**
       * @brief Erase the arc a->b
       * 
       * @param a id fo the first node
       * @param b id fo the second node
       */
      void eraseCausalArc(const std::string& a, const std::string& b){
         _ca_BN_.eraseArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
      }

      /**
       * @brief Add the arc a->b
       * 
       * @param a
       * @param b 
       */
      void addCausalArc(gum::NodeId a, gum::NodeId b){
         _ca_BN_.addArc(a, b);
      }

      /**
       * @brief Add the arc a->b
       * 
       * @param a
       * @param b 
       */
      void addCausalArc(const std::string& a, const std::string& b){
         _ca_BN_.addArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
      }

      /**
       * @brief Check if the arc a->b exists
       * 
       * @param a
       * @param b 
       */
      void existsArc(gum::NodeId a, gum::NodeId b){
         _ca_BN_.existsArc(a, b);
      }

      /**
       * @brief Check if the arc a->b exists
       * 
       * @param a
       * @param b 
       */
      void existsArc(const std::string& a, const std::string& b){
         _ca_BN_.existsArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
      }

      /**
       * @brief Return the set of nodes.
       * 
       * @return const gum::NodeSet& 
       */
      const gum::NodeSet& nodes() const {
         return _ca_BN_.nodes();
      }

      /**
       * @brief Return the set of arcs.
       * 
       * @return const gum::ArcSet& 
       */
      const gum::ArcSet& arcs() const {
         return _ca_BN_.arcs();
      }

      /**
       * @brief Create a dot representation of the causal model.
       * 
       * @return std::string 
       */
      std::string toDot(){
         std::string res = "digraph {";
         
         // latent variables

         return res;
      }
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
