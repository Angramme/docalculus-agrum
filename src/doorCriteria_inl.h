

namespace gum{
    template<typename iter>
    INLINE DoorIterable<iter>::DoorIterable(iter&& begin, iter&& end) 
        : _begin(begin), _end(end) 
        { GUM_CONSTRUCTOR(DoorIterable) } 
    
    template<typename iter>
    INLINE DoorIterable<iter>::DoorIterable() 
        : _begin(), _end() 
        { GUM_CONSTRUCTOR(DoorIterable) }
    
    template<typename iter>
    INLINE DoorIterable<iter>::~DoorIterable() { GUM_DESTRUCTOR(DoorIterable) };
    
    template<typename iter>
    INLINE DoorIterable<iter>::DoorIterable(DoorIterable<iter>&& v) 
        : _begin(std::move(v._begin)), _end(std::move(v._end)) 
        { GUM_CONS_MOV(DoorIterable) };
    
    template<typename iter>
    INLINE DoorIterable<iter>::DoorIterable(const FrontdoorIterator<iter>& v) 
        : _begin(v._begin), _end(v._end) 
        { GUM_CONS_CPY(DoorIterable) };
    
    template<typename iter>
    INLINE DoorIterable<iter>& DoorIterable<iter>::operator=(DoorIterable<iter>&& v) 
    { _begin = std::move(v._begin); _end = std::move(v._end); GUM_OP_MOV(DoorIterable); return *this; };
    
    template<typename iter>
    INLINE DoorIterable<iter>& DoorIterable<iter>::operator=(const DoorIterable<iter>& v) 
    { _begin = v._begin; _end = v._end; GUM_OP_CPY(DoorIterable); return *this; };
    
    template<typename iter>
    INLINE iter DoorIterable<iter>::begin() const 
    { return _begin; }; 
    
    template<typename iter>
    INLINE iter DoorIterable<iter>::end() const 
    { return _end; }; 

}