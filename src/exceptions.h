#ifndef _CAUSAL_EXCEPTIONS_H_
#define _CAUSAL_EXCEPTIONS_H_


#include <iostream>
#include <agrum/tools/core/set.h>
#include <string>

namespace gum{

    /**
     * @brief Represents an hedge exception for a causal query
     *
     * @param msg str :
     * @param observables NameSet :
    */
    class HedgeException : public std::exception {
    private:
        std::string message;
        Set<std::string> observables;
    public:
        INLINE const Set<std::string>& get_observables() const;
        char* what();
        
        /**
         * @brief Represents an hedge exception for a causal query
         *
         * @param msg str :
         * @param observables NameSet :
        */
        INLINE HedgeException(const char* msg, Set<std::string> observables);
        INLINE ~HedgeException();
    };

    /**
     * @brief Represents an unidentifiability for a causal query
     *
     * @param msg 
    */
    class UnidentifiableException : public std::exception {
    private:
        std::string message;
    public:
        INLINE char* what();
        /**
         * @brief Represents an unidentifiability for a causal query
         *
         * @param msg 
         */
        INLINE UnidentifiableException(const char* msg);
        INLINE ~UnidentifiableException();
    };
}

#ifndef GUM_NO_INLINE
#include "exceptions_inl.h"
#endif

#endif