
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

      GUM_CONSTRUCTOR(CausalModel);
   }

   template <typename GUM_SCALAR>
   CausalModel<GUM_SCALAR>::CausalModel(const CausalModel& ot)
      : _ob_BN_(ot._ob_BN_), _keepArcs_(ot._keepArcs_), _ca_BN_(ot._ca_BN_), _lat_(ot._lat_), _names_(ot._names_)
   {
      GUM_CONS_CPY(CausalModel);
   }

   template <typename GUM_SCALAR>
   CausalModel<GUM_SCALAR>::~CausalModel(){
      GUM_DESTRUCTOR(CausalModel);
   }

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
   NodeSet CausalModel<GUM_SCALAR>::children(const NodeSet& ids) const{
      return _ca_BN_.children(ids);
   }
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::children(const std::vector< std::string >& names) const{
      return _ca_BN_.children(names);
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
      return _ca_BN_.variableNodeMap();
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
   std::optional<gum::NodeSet> CausalModel<GUM_SCALAR>::backDoor(std::string cause, std::string effect){
      return backDoor(idFromName(cause), idFromName(effect));
   }

   template<typename GUM_SCALAR>
   std::optional<gum::NodeSet> CausalModel<GUM_SCALAR>::backDoor(gum::NodeId cause, gum::NodeId effect){
      for(auto bd : backdoor_generator(*this, cause, effect, latentVariablesIds())){
         return bd;
      }
      return std::nullopt;
   }

   template<typename GUM_SCALAR>
   std::optional<std::set<std::string>> CausalModel<GUM_SCALAR>::backDoor_withNames(std::string cause, std::string effect){
      return backDoor_withNames(idFromName(cause), idFromName(effect));
   }

   template<typename GUM_SCALAR>
   std::optional<std::set<std::string>> CausalModel<GUM_SCALAR>::backDoor_withNames(gum::NodeId cause, gum::NodeId effect){
      for(auto bd : backdoor_generator(*this, cause, effect, latentVariablesIds())){
         auto st = std::set<std::string>();
         for(auto i : bd){
            st.insert(observationalBN().variable(i).name());
         }
         return st;
      }
      return std::nullopt;
   }


   template<typename GUM_SCALAR>
   std::optional<gum::NodeSet> CausalModel<GUM_SCALAR>::frontDoor(std::string cause, std::string effect){
      return frontDoor(idFromName(cause), idFromName(effect));
   }

   template<typename GUM_SCALAR>
   std::optional<gum::NodeSet> CausalModel<GUM_SCALAR>::frontDoor(gum::NodeId cause, gum::NodeId effect){
      for(auto bd : frontdoor_generator(*this, cause, effect, latentVariablesIds())){
         return bd;
      }
      return std::nullopt;
   }

   template<typename GUM_SCALAR>
   std::optional<std::set<std::string>> CausalModel<GUM_SCALAR>::frontDoor_withNames(std::string cause, std::string effect){
      return frontDoor_withNames(idFromName(cause), idFromName(effect));
   }

   template<typename GUM_SCALAR>
   std::optional<std::set<std::string>> CausalModel<GUM_SCALAR>::frontDoor_withNames(gum::NodeId cause, gum::NodeId effect){
      for(auto bd : frontdoor_generator(*this, cause, effect, latentVariablesIds())){
         auto st = std::set<std::string>();
         for(auto i : bd){
            st.insert(observationalBN().variable(i).name());
         }
         return st;
      }
      return std::nullopt;
   }


   template <typename GUM_SCALAR>
   const DAG& CausalModel<GUM_SCALAR>::dag() const{
      return _ca_BN_.dag();
   }

   
   template <typename GUM_SCALAR>
   Size CausalModel<GUM_SCALAR>::size() const{
      return _ca_BN_.size();
   }

   
   template <typename GUM_SCALAR>
   Size CausalModel<GUM_SCALAR>::sizeArcs() const{
      return _ca_BN_.sizeArcs();
   }

   
   template <typename GUM_SCALAR>
   bool CausalModel<GUM_SCALAR>::exists(NodeId node) const{
      return _ca_BN_.exists(node);
   }

   
   template <typename GUM_SCALAR>
   bool CausalModel<GUM_SCALAR>::exists(const std::string& name) const{
      return _ca_BN_.exists(name);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::family(const NodeId id) const{
      return _ca_BN_.family(id);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::family(const std::string& name) const{
      return _ca_BN_.family(name);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::descendants(const NodeId id) const{
      return _ca_BN_.descendants(id);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::descendants(const std::string& name) const{
      return _ca_BN_.descendants(name);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::ancestors(const NodeId id) const{
      return _ca_BN_.ancestors(id);
   }

   
   template <typename GUM_SCALAR>
   NodeSet CausalModel<GUM_SCALAR>::ancestors(const std::string& name) const{
      return _ca_BN_.ancestors(name);
   }

   
   template <typename GUM_SCALAR>
   UndiGraph CausalModel<GUM_SCALAR>::moralizedAncestralGraph(const NodeSet& nodes) const{
      return _ca_BN_.moralizedAncestralGraph(nodes);
   }

   
   template <typename GUM_SCALAR>
   UndiGraph CausalModel<GUM_SCALAR>::moralizedAncestralGraph(const std::vector< std::string >& nodenames) const{
      return _ca_BN_.moralizedAncestralGraph(nodenames);
   }

   
   template <typename GUM_SCALAR>
   bool CausalModel<GUM_SCALAR>::isIndependent(NodeId X, NodeId Y, const NodeSet& Z) const{
      return _ca_BN_.isIndependent(X, Y, Z);
   }

   
   template <typename GUM_SCALAR>
   bool CausalModel<GUM_SCALAR>::isIndependent(const NodeSet& X, const NodeSet& Y, const NodeSet& Z) const{
      return _ca_BN_.isIndependent(X, Y, Z);
   }

   
   template <typename GUM_SCALAR>
   UndiGraph CausalModel<GUM_SCALAR>::moralGraph() const{
      return _ca_BN_.moralGraph();
   }

   
   template <typename GUM_SCALAR>
   Sequence< NodeId > CausalModel<GUM_SCALAR>::topologicalOrder() const{
      return _ca_BN_.topologicalOrder();
   }

   
   template <typename GUM_SCALAR>
   bool CausalModel<GUM_SCALAR>::hasSameStructure(const DAGmodel& other){
      return _ca_BN_.hasSameStructure(other);
   }

   
   template <typename GUM_SCALAR>
   CausalModel<GUM_SCALAR>& CausalModel<GUM_SCALAR>::operator=(const CausalModel<GUM_SCALAR>& source){
      _ob_BN_ = source._ob_BN_;
      _ca_BN_ = source._ca_BN_;
      _keepArcs_ = source._keepArcs_;
      _lat_ = source._lat_;
      _names_ = source._names_;
   }

   


   template<typename GUM_SCALAR>
   CausalModel<GUM_SCALAR> inducedCausalSubModel(const CausalModel<GUM_SCALAR>& cm){
      return inducedCausalSubModel(cm, cm.nodes());
   }

   template<typename GUM_SCALAR>
   CausalModel<GUM_SCALAR> inducedCausalSubModel(const CausalModel<GUM_SCALAR>& cm, const NodeSet& sns){
      const auto& nodes = sns - cm.latentVariablesIds();
      auto bn = BayesNet<GUM_SCALAR>();

      for(auto n : nodes){
         bn.add(cm.observationalBN().variable(n), n);
      }
      for(auto arc : cm.arcs()){
         if(nodes.contains(arc.tail) && nodes.contains(arc.head)){
            bn.addArc(arc.head, arc.tail);
         }
      }

      auto latentVarsDescriptor = std::vector<std::pair<std::string, std::vector<gum::NodeId>>>();
      const auto& names = cm.names();
      for(auto latentVar : cm.latentVariablesIds()){
         auto inters = cm.children(latentVar) * nodes;
         if(inters.size() > 0){
            std::vector<gum::NodeId> inters_vec(inters.begin(), inters.end());
            latentVarsDescriptor.push_back(std::make_pair(names[latentVar], inters_vec));
         } 
      }
      return CausalModel<GUM_SCALAR>(bn, latentVarsDescriptor, true);
   }
}
