#include "doorCriteria.h"

namespace gum{
    backdoor_iterator::reference backdoor_iterator::operator*() const { return *_cur_; }
    backdoor_iterator::pointer backdoor_iterator::operator->() const {return _cur_.get();}
    backdoor_iterator& backdoor_iterator::operator++() {_next_(); return *this;}
    backdoor_iterator backdoor_iterator::operator++(int) {backdoor_iterator tmp = *this; ++(*this); return tmp;}
    backdoor_iterator& backdoor_iterator::operator=(backdoor_iterator&& o) {
        _G_ = o._G_; 
        _backdoors_ = o._backdoors_; 
        _possible_ = o._possible_; 
        _cause_ = o._cause_;
        _effect_ = o._effect_; 
        _v_ = o._v_; 
        _cur_ = o._cur_;
        return *this;
    }
    bool operator==(const backdoor_iterator& a, const backdoor_iterator& b) {
        return false; // TODO impelemnt
    }
    bool operator==(const backdoor_iterator& a, backdoor_iterator_sentinel) {
        return false; // TODO impelemnt
    }
    bool operator!=(const backdoor_iterator& a, const backdoor_iterator& b) { return !operator==(a, b); }
    bool operator!=(const backdoor_iterator& a, backdoor_iterator_sentinel) { return !operator==(a, backdoor_iterator::end); }
    
    backdoor_iterator::backdoor_iterator(std::shared_ptr<DAG> G, std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect)
       :  _G_(G), _backdoors_(), _possible_(possible), _cause_(cause), _effect_(effect), _v_(), _cur_()
    {
        _comb_(1);
    }

    backdoor_iterator::backdoor_iterator(backdoor_iterator& o)
        : _G_(o._G_), _backdoors_(o._backdoors_), _possible_(o._possible_), _cause_(o._cause_),
        _effect_(o._effect_), _v_(o._v_), _cur_(o._cur_)
    {}
    backdoor_iterator::backdoor_iterator(backdoor_iterator&& o)
        : _G_(o._G_), _backdoors_(o._backdoors_), _possible_(o._possible_), _cause_(o._cause_),
        _effect_(o._effect_), _v_(o._v_), _cur_(o._cur_)
    {}

    void backdoor_iterator::_comb_(size_t r){
        *_v_ = std::vector<bool>(_possible_->size(), false);
        std::fill(_v_->end() - r, _v_->end(), true);
    }
    void backdoor_iterator::_next_(){

    }

}