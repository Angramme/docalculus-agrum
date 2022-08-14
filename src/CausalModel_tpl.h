
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/tools/graphs/parts/nodeGraphPart.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/core/hashTable.h>
#include <utility>
#include <string>

#include "CausalModel.h"

namespace gum{
   template <typename GUM_SCALAR>
   CausalModel<GUM_SCALAR>::CausalModel(const gum::BayesNet<GUM_SCALAR>& bn,
               const std::vector<std::pair<std::string, std::vector<gum::NodeId>>>& latentVarDescriptors,
               bool keepArcs
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

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::addLatentVariable(const std::string& name, const std::vector<std::string>& lchild, bool keepArcs){
      std::vector<gum::NodeId> ids(lchild.size());
      for(size_t i = 0; i<lchild.size(); i++) ids[i] = _ob_BN_.idFromName(lchild[i]);
      addLatentVariable(name, ids, keepArcs);
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::addLatentVariable(const std::string& name, const std::vector<gum::NodeId>& lchild, bool keepArcs){
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

   template <typename GUM_SCALAR>
   const gum::BayesNet<GUM_SCALAR>& CausalModel<GUM_SCALAR>::causalBN() const {
      return _ca_BN_;
   }

   template <typename GUM_SCALAR>
   const gum::BayesNet<GUM_SCALAR>& CausalModel<GUM_SCALAR>::observationalBN() const {
      return _ob_BN_;
   }

   template <typename GUM_SCALAR>
   const gum::NodeSet& CausalModel<GUM_SCALAR>::parents(std::string name) const {
      return parents(_ca_BN_.idFromName(name));
   }

   template <typename GUM_SCALAR>
   const gum::NodeSet& CausalModel<GUM_SCALAR>::parents(gum::NodeId id) const {
      return _ca_BN_.parents(id);
   }

   template <typename GUM_SCALAR>
   const gum::NodeSet& CausalModel<GUM_SCALAR>::children(std::string name) const {
      return _ca_BN_.children(_ca_BN_.idFromName(name));
   }

   template <typename GUM_SCALAR>
   const gum::NodeSet& CausalModel<GUM_SCALAR>::children(gum::NodeId id) const {
      return _ca_BN_.children(id);
   }

   template <typename GUM_SCALAR>
   const decltype(CausalModel<GUM_SCALAR>::_names_)& CausalModel<GUM_SCALAR>::names() const {
      return _names_;
   }

   template <typename GUM_SCALAR>
   gum::NodeId CausalModel<GUM_SCALAR>::idFromName(const std::string& name) const {
      return _ca_BN_.idFromName(name);
   }

   template <typename GUM_SCALAR>
   const gum::NodeSet& CausalModel<GUM_SCALAR>::latentVariablesIds() const {
      return _lat_;
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::eraseCausalArc(gum::NodeId a, gum::NodeId b){
      _ca_BN_.eraseArc(a, b);
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::eraseCausalArc(const std::string& a, const std::string& b){
      _ca_BN_.eraseArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::addCausalArc(gum::NodeId a, gum::NodeId b){
      _ca_BN_.addArc(a, b);
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::addCausalArc(const std::string& a, const std::string& b){
      _ca_BN_.addArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
   }

   template <typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::existsArc(gum::NodeId a, gum::NodeId b){
      _ca_BN_.existsArc(a, b);
   }

   template<typename GUM_SCALAR>
   void CausalModel<GUM_SCALAR>::existsArc(const std::string& a, const std::string& b){
      _ca_BN_.existsArc(_ob_BN_.idFromName(a), _ob_BN_.idFromName(b));
   }

   template<typename GUM_SCALAR>
   decltype(CausalModel<GUM_SCALAR>::_ca_BN_.nodes()) CausalModel<GUM_SCALAR>::nodes() const {
      return _ca_BN_.nodes();
   }

   template<typename GUM_SCALAR>
   const gum::ArcSet& CausalModel<GUM_SCALAR>::arcs() const {
      return _ca_BN_.arcs();
   }

   template<typename GUM_SCALAR>
   const gum::VariableNodeMap& CausalModel<GUM_SCALAR>::variableNodeMap() const {
      return _ca_BN_.variableNodeMap(); // TODO: implement this !
   }

   template<typename GUM_SCALAR>
   const gum::DiscreteVariable& CausalModel<GUM_SCALAR>::variable(gum::NodeId x) const {
      return _ca_BN_.variable(x);
   }

   template<typename GUM_SCALAR>
   gum::NodeId CausalModel<GUM_SCALAR>::nodeId(const gum::DiscreteVariable& x) const {
      return _ca_BN_.nodeId(x);
   }

   template<typename GUM_SCALAR>
   const gum::DiscreteVariable& CausalModel<GUM_SCALAR>::variableFromName(const std::string& name) const{
      return _ca_BN_.variableFromName(name);
   }


   template<typename GUM_SCALAR>
   std::string CausalModel<GUM_SCALAR>::toDot(){
      std::string res = "digraph {";
      
      // latent variables
      // TODO: finish

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
