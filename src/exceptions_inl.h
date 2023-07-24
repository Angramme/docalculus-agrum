

namespace gum{
    
    INLINE const Set<std::string>& HedgeException::get_observables() const { return observables; }
    
    INLINE HedgeException::HedgeException(const char* msg, Set<std::string> observables)
        : message("HedgeException : "), observables(observables)
    {
        message += msg;
        GUM_CONSTRUCTOR(HedgeException)
    }
    
    INLINE HedgeException::~HedgeException(){
        GUM_DESTRUCTOR(HedgeException)
    }


    INLINE char* UnidentifiableException::what(){
        return const_cast<char*>(message.c_str());
    }

    INLINE UnidentifiableException::UnidentifiableException(const char* msg)
        : message("UnidentifiableException : ")
    {
        message += msg;
        GUM_CONSTRUCTOR(UnidentifiableException)
    }
    
    INLINE UnidentifiableException::~UnidentifiableException(){
        GUM_DESTRUCTOR(UnidentifiableException)
    }
}