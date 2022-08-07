
#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/undiGraph.h>

#include "CausalModel.h"
#include "dSeparation.h"

namespace gum{

    template<typename GUM_SCALAR>
    bool isParent(NodeId a, NodeId b, const BayesNet<GUM_SCALAR>& bn){
        return bn.existsArc(a, b);
    }

    template<typename GUM_SCALAR>
    UndiGraph reduce_moralize(const BayesNet<GUM_SCALAR>& bn, const NodeSet& x, const NodeSet& y, const NodeSet& zset){
        auto G = UndiGraph();

        auto ancetre = x + y;
        const auto anc  = ancetre;
        for(const auto& i : anc){
            ancetre += bn.ancestors(i);
        }

        for(const auto& i : zset){
            ancetre.insert(i);
            ancetre += bn.ancestors(i);
        }
        
        for(const auto& i : ancetre){
            G.addNodeWithId(i);
        }

        for(const auto& b : G.nodes()){
            for(const auto& a : bn.parents(b)){
                G.addEdge(a, b);
            }
        }

        for(const auto& nod : G.nodes()){
            auto parents_nod = bn.parents(nod);
            for(const auto& par : parents_nod){
                for(const auto& par2 : parents_nod){
                    if(par == par2) continue;
                    G.addEdge(par, par2);
                }
            }
        }

        return G;
    }

    template<typename GUM_SCALAR>
    bool isDSep_tech2(const BayesNet<GUM_SCALAR>& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset){
        auto gu = reduce_moralize(bn, sx, sy, zset);
        remove_nodes(gu, zset);
        return !is_path_x_y(gu, sx, sy);
    }

    template<typename GUM_SCALAR>
    bool isDSep_parents(const BayesNet<GUM_SCALAR>& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset){
        return _isDSep_tech2_parents(bn, sx, sy, zset);
    }

    template<typename GUM_SCALAR>
    bool isDSep_tech2_parents(const BayesNet<GUM_SCALAR>& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset){
        auto G = UndiGraph();
        auto ancestors = sx + sy;
        const auto anc = ancestors;
        for(const auto& i : anc){
            ancestors += bn.ancestors(i);
        }

        for(const auto& i : zset){
            ancestors.insert(i);
            ancestors += bn.ancestors(i);
        }
        for(const auto& i : ancestors){
            G.addNodeWithId(i);
        }

        for(const auto& b : G.nodes()){
            for(const auto& a : (bn.parents(b) - sx)){
                G.addEdge(a, b);
            }
        }
        for(const auto& nod : G.nodes()){
            const auto parents_nod = bn.parents(nod) - sx;
            for(const auto& par : parents_nod){
                for(const auto& par2 : parents_nod){
                    if(par == par2) continue;
                    G.addEdge(par, par2);
                }
            }
        }
        remove_nodes(G, zset);

        return !is_path_x_y(bn, sx, sy);
    }


    template<typename GUM_SCALAR>
    bool isDSep_tech2_children(const BayesNet<GUM_SCALAR>& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset){
        auto G = UndiGraph();
        auto ancestors = sx + sy;
        for(const auto& i : sy){
            ancestors += bn.ancestors(i);
        }
        // sx's ancesters will not be added since sx already is in ancesters
        for(const auto& i : zset){
            ancestors.insert(i);
            ancestors += bn.ancestors(i);
        }
        for(const auto& i : ancestors){
            G.addNodeWithId(i);
        }
        auto se = G.nodes() - sx;
        for(const auto& b : se){
            for(const auto& a : bn.parents(b))
                G.addEdge(a, b);
        }
        for(const auto& nod : se){
            const auto parents_nod = bn.parents(nod);
            for(const auto& par : parents_nod){
                for(const auto& par2 : parents_nod){
                    if(par == par2) continue;
                    G.addEdge(par, par2);
                }
            }
        }
        remove_nodes(G, zset);
    
        return !is_path_x_y(G, sx, sy);
    }


    template<typename GUM_SCALAR>
    bool is_descendant(const BayesNet<GUM_SCALAR>& bn, NodeId x, NodeId y, const NodeSet& marked){
        if(isParent(y, x, bn)) return true;
        
        for(const auto& c : bn.children(y)){
            if(marked.contains(c)) continue;
            marked.insert(c);
            if(is_descendant(bn, x, c, marked)) return true;
        }
        return false;
    }
}