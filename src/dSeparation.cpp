#include "dSeparation.h"
#include <utility>

namespace gum{
    bool _IPXY_inner_ec(const UndiGraph& gu, NodeId a, const NodeSet& b, NodeSet& m){
        if(b.contains(a)) return true;
        m.insert(a);
        for(const auto& n : gu.neighbours(a)){
            if(m.contains(n)) continue;
            return _IPXY_inner_ec(gu, n, b, m);
        }
        return false;
    }

    bool is_path_x_y(const UndiGraph& gg, const NodeSet& sx, const NodeSet& sy, const NodeSet& marked){
        const NodeSet *ssx = &sx, *ssy = &sy;
        if(sx.size() > sy.size()) std::swap(ssx, ssy);

        auto ma = NodeSet(marked);
        for(const auto& i : *ssx){
            ma.insert(i);
            if(_IPXY_inner_ec(gg, i, *ssy, ma)) return true;
        }
        return false;
    }

    void remove_nodes(UndiGraph& gg, const NodeSet& zset){
        for(const auto& node : gg.nodes()){
            if(!zset.contains(node)) continue;
            gg.eraseNode(node);
        }
    }
}
