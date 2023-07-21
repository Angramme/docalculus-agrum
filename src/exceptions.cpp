#include "exceptions.h"

namespace gum{

    char* HedgeException::what(){
        if(observables.size() == 0) return const_cast<char*>(message.c_str());
        std::string msg = message + "\n\tobservables:\n";
        for(const auto& o : observables){
            msg += "\n\t - " + o;
        }
        return const_cast<char*>(message.c_str());
    }
}