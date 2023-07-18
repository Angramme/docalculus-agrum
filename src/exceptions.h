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
        inline const Set<std::string>& get_observables() const { return observables; }
        char* what(){
            if(observables.size() == 0) return const_cast<char*>(message.c_str());
            std::string msg = message + "\n\tobservables:\n";
            for(const auto& o : observables){
                msg += "\n\t - " + o;
            }
            return const_cast<char*>(message.c_str());
        }
        
        /**
         * @brief Represents an hedge exception for a causal query
         *
         * @param msg str :
         * @param observables NameSet :
        */
        HedgeException(const char* msg, Set<std::string> observables)
            : message("HedgeException : "), observables(observables)
        {
            message += msg;
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
        char* what(){
            return const_cast<char*>(message.c_str());
        }
        /**
         * @brief Represents an unidentifiability for a causal query
         *
         * @param msg 
         */
        UnidentifiableException(const char* msg)
            : message("UnidentifiableException : ")
        {
            message += msg;
        }
    };
}