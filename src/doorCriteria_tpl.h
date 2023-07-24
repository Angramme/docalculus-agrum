#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/set.h>
#include <string>

#include "doorCriteria.h"
#include "dSeparation.h"

namespace gum{
    template<typename GUM_SCALAR>
    bool backdoor_path(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset){
        return !isDSep_parents(bn, x, y, zset);
    }

    template<typename GUM_SCALAR>
    bool exists_unblocked_directed_path(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y,
        const gum::Set<std::string>& zset)
        {
        if(bn.children(x).contains(y)) return true;

        for(const auto& c : bn.children(x)){
            if(!zset.contains(c) && exists_unblocked_directed_path(bn, c, y, zset)) return true; 
        }
        return false;
    }

    template<typename GUM_SCALAR>
    bool is_frontdoor(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y, 
        const gum::Set<std::string>& zset)
        {
        if(exists_unblocked_directed_path(bn, x, y, zset)) return false;
        if((zset * backdoor_reach(bn, x)).size() != 0) return false;
        const auto bn_reduit = dSep_reduce(bn, zset + gum::Set({x, y}));
        const auto xset = gum::Set({x});
        for(const auto& i : zset){
            if(backdoor_path(bn_reduit, i, y, xset)) return false;
        }
        return true;
    }

    template<typename GUM_SCALAR>
    bool is_backdoor(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset){
        const auto dex = descendants(bn, x);
        if((dex * zset).size() != 0) return false;
        
        return isDSep_parents(bn, x, y, zset);
    }

    template<typename GUM_SCALAR>
    void _BR_inner_br(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        gum::NodeId x, bool pht, 
        gum::NodeSet& reach0, 
        gum::NodeSet& reach1)
        {
        for(const auto& c : bn.children(x)){
            if(reach0.contains(c) || reach1.contains(c)) continue;
            reach1.insert(c);
            _BR_inner_br(bn, c, true, reach0, reach1);
        }
        if(pht) return;
        for(const auto& p : bn.parents(x)){
            if(reach0.contains(p)) continue;
            reach0.insert(p);
            _BR_inner_br(bn, p, false, reach0, reach1);
        }
    }

    template<typename GUM_SCALAR>
    gum::NodeSet backdoor_reach(const gum::BayesNet<GUM_SCALAR>& bn, gum::NodeId a){
        const auto r = gum::Set({a});
        r += bn.parents(a);
        const auto l = gum::Set({a});
        for(const auto& pa : bn.parents(a)){
            _BR_inner_br(bn, pa, false, r, l);
        }
        auto s = r + l;
        if(s.contains(a)) s.erase(a);
        return s;
    }

    template<typename GUM_SCALAR>
    std::unique_ptr<NodeSet> _NOD_inner_nod(BayesNet<GUM_SCALAR> bn, NodeId a, NodeId b){
        if(a == b) return std::make_unique<NodeSet>();
        auto inners = std::make_unique<NodeSet>({a});
        auto chds = bn.children(a);
        if(chds.size() == 0) return nullptr;
        bool found = false;
        for(const auto& c : chds){
            auto s = _NOD_inner_nod(bn, c, b);
            if(s){
                found = true;
                *inners += *s;
            }
        }
        if(found) return std::move(inners);
        return nullptr;
    }

    template<typename GUM_SCALAR>
    std::unique_ptr<NodeSet> nodes_on_dipath(const BayesNet<GUM_SCALAR>& bn, NodeId x, NodeId y){
        auto r = _NOD_inner_nod(bn, x, y);
        if(r) r.remove(x);
        return std::move(r);
    }

    template<typename GUM_SCALAR> // TODO: giga tester ca
    BackdoorIterable backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd){
        if(bn.parents(cause).size() == 0) return BackdoorIterable(); // empty
        if(isParent(effect, cause, bn)) return BackdoorIterable(); // empty

        // simplify the graph
        auto interest = NodeSet({cause, effect});
        auto G = std::make_shared<DAG>(dSep_reduce(bn, interest));

        {
            // removing the non connected in G without descendants
            // GG is a trash graph just to find the disjointed nodes in G
            auto GG = DiGraph(*G);
            for(const auto& i : descendants(bn, cause)) GG.eraseNode(i);

            // we only keep interesting connex components
            for(const auto& [_, nodes] : GG.connectedComponents()){
                if((interest + nodes).size() != 0) continue;
                for(const auto& n : nodes) G->eraseNode(n);
            }
        }

        std::shared_ptr<NodeSet> possible = std::make_shared<NodeSet>();
        *possible = G->nodes().asNodeSet() - (descendants(bn, cause) + interest + not_bd);
        if(possible->size() == 0) return BackdoorIterable();

        return BackdoorIterable(BackdoorIterator(G, possible, cause, effect), BackdoorIterator());
    }

    template<typename GUM_SCALAR> // TODO: giga tester ca
    FrontdoorIterable<GUM_SCALAR> frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_fd){
        if(isParent(cause, effect, bn)) return FrontdoorIterable(); // empty
        auto possible = nodes_on_dipath(bn, cause, effect);
        bool nodiPath = false;
        if(possible){
            nodiPath = true;
            possible = std::make_unique<NodeSet>();
            for(const auto& i : bn.nodes()) 
                if(i != cause && i != effect) possible->insert(i);
        }
        *possible -= backdoor_reach(bn, cause);
        *possible -= not_fd;
        auto impossible = NodeSet();
        auto g = dSep_reduce(bn, Set({cause, effect}) + *possible);
        for(const auto& z : *possible){
            if(!backdoor_path(g, Set({z}), Set({effect}), Set({cause}))) continue;
            impossible.insert(z);
        }
        *possible -= impossible;

        return FrontdoorIterable(FrontdoorIterator(bn, *possible, cause, effect, nodiPath), FrontdoorIterator());
    }

    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>::FrontdoorIterator
        (const std::shared_ptr<BayesNet<GUM_SCALAR>> bn, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect, bool nodiPath)
        : DoorIterator(
            false, 
            true,
            nullptr, 
            possible, 
            cause, 
            effect, 
            gum::Set<NodeSet>(), 
            std::vector<bool>(possible->size(), false), 
            0,
            NodeSet({})), 
            _bn_(bn),
            _nodiPath_(nodiPath)

    {
        GUM_CONSTRUCTOR(FrontdoorIterator)
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>::FrontdoorIterator()
        : DoorIterator(true), _bn_(nullptr), _nodiPath_(false)
    {    
        GUM_CONSTRUCTOR(FrontdoorIterator)
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>::FrontdoorIterator(FrontdoorIterator<GUM_SCALAR>&& v)
        : DoorIterator(v),  _bn_(std::move(v._bn_)), _nodiPath_(std::exchange(v._nodiPath_, false))
    {
        GUM_CONS_MOV(FrontdoorIterator)
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>::FrontdoorIterator(const FrontdoorIterator<GUM_SCALAR>& v)
        : DoorIterator(v),  _bn_(v._bn_), _nodiPath_(v._nodiPath_)
    {
        GUM_CONS_CPY(FrontdoorIterator)
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>::~FrontdoorIterator(){
        GUM_DESTRUCTOR(FrontdoorIterator)
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>& FrontdoorIterator<GUM_SCALAR>::operator=(FrontdoorIterator<GUM_SCALAR>&& v){
        DoorIterator::operator=(v);
        _bn_ = std::move(v._bn_);
        _nodiPath_ = std::exchange(v._nodiPath_, false);
        GUM_OP_MOV(FrontdoorIterator)
        return *this;
    }
    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>& FrontdoorIterator<GUM_SCALAR>::operator=(const FrontdoorIterator<GUM_SCALAR>& v){
        DoorIterator::operator=(v);
        _bn_ = v._bn_;
        _nodiPath_ = v._nodiPath_;
        GUM_OP_CPY(FrontdoorIterator)
        return *this;
    }


    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR>& FrontdoorIterator<GUM_SCALAR>::operator++() {
        if(_next_()) return *this;
        _is_the_end_ = true;
        return *this;
    }

    template<typename GUM_SCALAR>
    FrontdoorIterator<GUM_SCALAR> FrontdoorIterator<GUM_SCALAR>::operator++(int) {
        FrontdoorIterator tmp = *this; ++(*this); 
        return tmp;
    }

    template<typename GUM_SCALAR>
    bool FrontdoorIterator<GUM_SCALAR>::_next_(){
        if(_nodiPath_){
            if(_selection_size_ >= _possible_->size()) return false;
            _cur_ = Set({(*_possible_)[_selection_size_++]});
            return true;
        }
        if(!_advance_selection_mask_()) return false; 
        _gen_cur_();
        bool worth_testing = true;
        for(auto& s : _doors_){
            if(s.isSubsetOrEqual(_cur_)) worth_testing = false;
        }
        if(worth_testing && !exists_unblocked_directed_path(_bn_, _cause_, _effect_, _cur_)){
            _doors_.insert(_cur_);
            return true;
        }else{
            return _next_(); // skip this as this is an invalid set
        }
    }

}