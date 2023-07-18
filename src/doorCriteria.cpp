#include "doorCriteria.h"
#include "dSeparation.h"

#include <agrum/tools/core/set.h>

namespace gum{
    door_iterator::reference door_iterator::operator*() const { 
        return _cur_; 
    }
    door_iterator::pointer door_iterator::operator->() const {
        return &_cur_;
    }
    backdoor_iterator& backdoor_iterator::operator++() {
        if(_next_()) return *this;
        _is_the_end_ = true;
        return *this;
    }
    backdoor_iterator backdoor_iterator::operator++(int) {
        backdoor_iterator tmp = *this; ++(*this); 
        return tmp;
    }
    backdoor_iterator& backdoor_iterator::operator=(backdoor_iterator&& o) {
        _is_the_end_ = o._is_the_end_;
        _is_front_door_ = o._is_front_door_;
        _G_ = o._G_;
        _possible_ = o._possible_;
        _cause_ = o._cause_;
        _effect_ = o._effect_;
        _doors_ = o._doors_;
        _selection_mask_ = o._selection_mask_;
        _selection_size_ = o._selection_size_;
        _cur_ = o._cur_;
        return *this;
    }

    bool operator==(const door_iterator& a, const door_iterator& b) {
        if(a._is_front_door_ != b._is_front_door_) return false;
        if(a._is_the_end_ && b._is_the_end_) return true;

        if(a._G_.get() != b._G_.get()) return false;
        if(a._possible_.get() != b._possible_.get()) return false;
        if(a._cause_ != b._cause_ || a._effect_ != b._effect_) return false;
        if(a._selection_size_ != b._selection_size_) return false;
        if(a._selection_mask_ != b._selection_mask_) return false;
        if(a._cur_ != b._cur_) return false;
        return a._doors_ == b._doors_;
    }
    bool operator!=(const door_iterator& a, const door_iterator& b) { 
        return !operator==(a, b); 
    }

    backdoor_iterator::backdoor_iterator
        (const std::shared_ptr<DAG> G, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect)
        : door_iterator(
            false, 
            false,
            G, 
            possible, 
            cause, 
            effect, 
            gum::Set<NodeSet>(), 
            std::vector<bool>(possible->size(), false), 
            0,
            NodeSet({}))
    {}

    bool backdoor_iterator::_next_(){
        if(!_advance_selection_mask_()) return false; 
        _gen_cur_();
        bool worth_testing = true;
        for(auto& s : _doors_){
            if(s.isSubsetOrEqual(_cur_)) worth_testing = false;
        }
        if(worth_testing && isDSep_parents(*_G_, NodeSet({_cause_}), NodeSet({_effect_}), _cur_)){
            _doors_.insert(_cur_);
            return true;
        }else{
            return _next_(); // skip this as this is an invalid set
        }
    }

    
    void door_iterator::_gen_cur_(){
        _cur_.clear();
        int i=0;
        for(const auto& el : *_possible_){
            if(!_selection_mask_[i++]) continue;
            _cur_.insert(el);
        }
    }
    bool door_iterator::_advance_selection_mask_(){
        if(_is_the_end_) return false;
        bool x = std::prev_permutation(_selection_mask_.begin(), _selection_mask_.end());
        if(x) return true;

        _selection_size_++;
        if(_selection_size_ > _selection_mask_.size()) return false;
        std::fill(_selection_mask_.begin(), _selection_mask_.begin() + _selection_size_, true);
        std::fill(_selection_mask_.begin() + _selection_size_, _selection_mask_.end() , false);
        return true;
    }
}