#include "doAST.h"
#include <iostream>
#include <string>

#ifdef GUM_NO_INLINE
#  include "doAST_inl.h"
#endif

namespace gum{

    void ___sort_comma_push_stream(std::ostream& outs, const Set<std::string>& VS){
        auto svs = std::vector<const std::string*>();
        for(const auto& v : VS) svs.push_back(&v);
        std::sort(svs.begin(), svs.end(), [](const std::string* a, const std::string* b){ return *a < *b;});
        if(svs.size() > 0) outs << *svs[0];
        for(size_t i=1; i<svs.size(); i++){
            outs << "," << *svs[i];
        }
    }
    void ___sort_comma_push_stream(std::ostream& outs, const std::vector<std::string>& VS){
        auto svs = std::vector<const std::string*>();
        for(const auto& v : VS) svs.push_back(&v);
        std::sort(svs.begin(), svs.end(), [](const std::string* a, const std::string* b){ return *a < *b;});
        if(svs.size() > 0) outs << *svs[0];
        for(size_t i=1; i<svs.size(); i++){
            outs << "," << *svs[i];
        }
    }
}
