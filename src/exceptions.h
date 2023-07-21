#ifndef _CAUSAL_EXCEPTIONS_H_
#define _CAUSAL_EXCEPTIONS_H_


#include <iostream>
#include <agrum/tools/core/set.h>
#include <string>

namespace gum{

    // TODO: move all inlines into separate files

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
        inline const Set<std::string>& get_observables() const { return observables; }
        char* what();
        
        /**
         * @brief Represents an hedge exception for a causal query
         *
         * @param msg str :
         * @param observables NameSet :
        */
        inline HedgeException(const char* msg, Set<std::string> observables)
            : message("HedgeException : "), observables(observables)
        {
            message += msg;
            GUM_CONSTRUCTOR(HedgeException)
        }
        inline ~HedgeException(){
            GUM_DESTRUCTOR(HedgeException)
        }
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
        inline char* what(){
            return const_cast<char*>(message.c_str());
        }
        /**
         * @brief Represents an unidentifiability for a causal query
         *
         * @param msg 
         */
        inline UnidentifiableException(const char* msg)
            : message("UnidentifiableException : ")
        {
            message += msg;
            GUM_CONSTRUCTOR(UnidentifiableException)
        }
        inline ~UnidentifiableException(){
            GUM_DESTRUCTOR(UnidentifiableException)
        }
    };
}

#endif