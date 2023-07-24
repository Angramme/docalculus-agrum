#include "doorCriteria.h"
#include "dSeparation.h"

#include <agrum/tools/core/set.h>

#ifdef GUM_NO_INLINE
#  include "doorCriteria_inl.h"
#endif
namespace gum{

    DoorIterator::DoorIterator(
        bool is_the_end,
        bool is_front_door,
        std::shared_ptr<DAG> G,
        std::shared_ptr<NodeSet> possible,
        NodeId cause,
        NodeId effect,
        Set<NodeSet> backdoors,
        std::vector<bool> selection_mask,
        size_t selection_size,
        value_type cur
    ) :
        _is_the_end_(is_the_end),
        _is_front_door_(is_front_door),
        _G_(G),
        _possible_(possible),
        _cause_(cause),
        _effect_(effect),
        _doors_(backdoors),
        _selection_mask_(selection_mask),
        _selection_size_(selection_size),
        _cur_(cur)
    {
        GUM_CONSTRUCTOR(DoorIterator)
    }
    
    DoorIterator::DoorIterator(bool is_frontdoor)
        : DoorIterator(true, is_frontdoor, nullptr, nullptr, 0, 0, Set<NodeSet>({}), std::vector<bool>({}), 0, NodeSet({}))
    {}

    DoorIterator::~DoorIterator(){
        GUM_DESTRUCTOR(DoorIterator)
    }

    DoorIterator::DoorIterator(DoorIterator&& v)
        : 
        _is_the_end_(std::exchange(v._is_the_end_, true)),
        _is_front_door_(std::exchange(v._is_front_door_, v._is_front_door_)),
        _G_(std::move(v._G_)),
        _possible_(std::move(v._possible_)),
        _cause_(std::exchange(v._cause_, 0)),
        _effect_(std::exchange(v._effect_, 0)),
        _doors_(std::move(v._doors_)),
        _selection_mask_(std::move(v._selection_mask_)),
        _selection_size_(std::exchange(v._selection_size_, 0)),
        _cur_(std::move(v._cur_))
    {
        GUM_CONS_MOV(DoorIterator)
    }

    DoorIterator::DoorIterator(const DoorIterator& v)
        : 
        _is_the_end_(v._is_the_end_),
        _is_front_door_(v._is_front_door_),
        _G_(v._G_),
        _possible_(v._possible_),
        _cause_(v._cause_),
        _effect_(v._effect_),
        _doors_(v._doors_),
        _selection_mask_(v._selection_mask_),
        _selection_size_(v._selection_size_),
        _cur_(v._cur_)
    {
        GUM_CONS_CPY(DoorIterator)
    }

    DoorIterator& DoorIterator::operator=(DoorIterator&& v){
        _is_the_end_ = std::exchange(v._is_the_end_, true);
        _is_front_door_ = std::exchange(v._is_front_door_, v._is_front_door_);
        _G_ = std::move(v._G_);
        _possible_ = std::move(v._possible_);
        _cause_ = std::exchange(v._cause_, 0);
        _effect_ = std::exchange(v._effect_, 0);
        _doors_ = std::move(v._doors_);
        _selection_mask_ = std::move(v._selection_mask_);
        _selection_size_ = std::exchange(v._selection_size_, 0);
        _cur_ = std::move(v._cur_);
        GUM_OP_MOV(DoorIterator)
        return *this;
    }

    DoorIterator& DoorIterator::operator=(const DoorIterator& v){
        _is_the_end_ = v._is_the_end_;
        _is_front_door_ = v._is_front_door_;
        _G_ = v._G_;
        _possible_ = v._possible_;
        _cause_ = v._cause_;
        _effect_ = v._effect_;
        _doors_ = v._doors_;
        _selection_mask_ = v._selection_mask_;
        _selection_size_ = v._selection_size_;
        _cur_ = v._cur_;
        GUM_OP_CPY(DoorIterator)
        return *this;
    }

    BackdoorIterator::BackdoorIterator
        (const std::shared_ptr<DAG> G, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect)
        : DoorIterator(
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
    {
        GUM_CONSTRUCTOR(BackdoorIterator)
    }
    BackdoorIterator::BackdoorIterator()
        : DoorIterator(false)
    {
        GUM_CONSTRUCTOR(BackdoorIterator)
    }
    BackdoorIterator::BackdoorIterator(BackdoorIterator&& v)
        : DoorIterator(v)
    {
        GUM_CONS_MOV(BackdoorIterator)
    }
    BackdoorIterator::BackdoorIterator(const BackdoorIterator& v)
        : DoorIterator(v)
    {
        GUM_CONS_CPY(BackdoorIterator)
    }
    BackdoorIterator::~BackdoorIterator() {
        GUM_DESTRUCTOR(BackdoorIterator)
    }
    BackdoorIterator& BackdoorIterator::operator=(BackdoorIterator&& o) {
        DoorIterator::operator=(o);
        GUM_OP_MOV(BackdoorIterator)
        return *this;
    }
    BackdoorIterator& BackdoorIterator::operator=(const BackdoorIterator& o) {
        DoorIterator::operator=(o);
        GUM_OP_CPY(BackdoorIterator)
        return *this;
    }
    

    DoorIterator::reference DoorIterator::operator*() const { 
        return _cur_; 
    }
    DoorIterator::pointer DoorIterator::operator->() const {
        return &_cur_;
    }
    BackdoorIterator& BackdoorIterator::operator++() {
        if(_next_()) return *this;
        _is_the_end_ = true;
        return *this;
    }
    BackdoorIterator BackdoorIterator::operator++(int) {
        BackdoorIterator tmp = *this; ++(*this); 
        return tmp;
    }

    bool operator==(const DoorIterator& a, const DoorIterator& b) {
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
    bool operator!=(const DoorIterator& a, const DoorIterator& b) { 
        return !operator==(a, b); 
    }

    bool BackdoorIterator::_next_(){
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

    
    void DoorIterator::_gen_cur_(){
        _cur_.clear();
        int i=0;
        for(const auto& el : *_possible_){
            if(!_selection_mask_[i++]) continue;
            _cur_.insert(el);
        }
    }
    bool DoorIterator::_advance_selection_mask_(){
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