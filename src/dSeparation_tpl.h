
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
        auto se = G.asNodeSet() - sx;
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

    template<typename GUM_SCALAR>
    void _BN_inner_rec(NodeSet& s, NodeId a, const BayesNet<GUM_SCALAR>& bn, const NodeSet& interest){
        if((interest + s).contains(a)) return;
        s.insert(a);
        for(const auto& b : bn.parents(a)){
            if((bn.children(b) - s).empty() && !s.contains(b))
                _BN_inner_rec(s, b, bn, interest);
        }
    }

    template<typename GUM_SCALAR>
    const NodeSet& barren_nodes(const BayesNet<GUM_SCALAR>& bn, const NodeSet& interest){
        auto s = NodeSet();
        
        for(const auto& x : bn.nodes()){
            if(bn.children(x).empty()) _BN_inner_rec(s, x, bn, interest);
        }

        return s;
    }


    template<typename GUM_SCALAR>
    DAG partialDAGfromBN(const BayesNet<GUM_SCALAR>& bn, const NodeSet& nexcl){
        auto d = DAG();

        auto nodes = bn.nodes() - nexcl;
        for(const auto& n : nodes){
            d.addNodeWithId(n);
        }

        for(const auto& xy : bn.arcs()){
            if(nodes.contains(xy.first()) && nodes.contains(xy.second())){
                d.addArc(xy);
            }
        }
        return d;
    }


    NodeSet _filaires(const DAG& bn, const NodeSet& interest, bool inf);

    template<typename GUM_SCALAR>
    DAG dSep_reduce(const BayesNet<GUM_SCALAR>& g, const NodeSet& interest){
        const auto barren = barren_nodes(g, interest);
        auto reduced_g = partialDAGfromBN(g, barren);
        for(const auto& f : _filaires(reduced_g, interest, false))
            reduced_g.eraseNode(f);

        return reduced_g;
    }

    // TODO:

    //   barren = _barren_nodes(g, interest)

    //   reduced_g = partialDAGFromBN(g, barren)

    //   for f in _filaires(reduced_g, interest, False):
    //     reduced_g.eraseNode(f)

    //   return reduced_g
}