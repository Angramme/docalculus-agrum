#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/set.h>
#include <string>

#include "doorCriteria.h"

namespace gum{
    template<typename GUM_SCALAR>
    bool backdoor_path(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset){
        return !isDSep_parents(bn, x, y, zset);
    }

    template<typename GUM_SCALAR>
    bool exists_unblocked_directed_path(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y,
        const gum::Set<std::string>& zset)
        {
        if(bn.children(x).contains(y)) return true;

        for(const auto& c : bn.children(x)){
            if(!zset.contains(c) && exists_unblocked_directed_path(bn, c, y, zset)) return true; 
        }
        return false;
    }

    template<typename GUM_SCALAR>
    bool is_frontdoor(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y, 
        const gum::Set<std::string>& zset)
        {
        if(exists_unblocked_directed_path(bn, x, y, zset)) return false;
        if((zset * backdoor_reach(bn, x)).size() != 0) return false;
        const auto bn_reduit = dSep_reduce(bn, zset + gum::Set({x, y}));
        const auto xset = gum::Set({x});
        for(const auto& i : zset){
            if(backdoor_path(bn_reduit, i, y, xset)) return false;
        }
        return true;
    }

    template<typename GUM_SCALAR>
    bool is_backdoor(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset){
        const auto dex = descendants(bn, x);
        if((dex * zset).size() != 0) return false;
        
        return isDSep_parents(bn, x, y, zset);
    }

    template<typename GUM_SCALAR>
    void _BR_inner_br(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        gum::NodeId x, bool pht, 
        gum::NodeSet& reach0, 
        gum::NodeSet& reach1)
        {
        for(const auto& c : bn.children(x)){
            if(reach0.contains(c) || reach1.contains(c)) continue;
            reach1.insert(c);
            _BR_inner_br(bn, c, true, reach0, reach1);
        }
        if(pht) return;
        for(const auto& p : bn.parents(x)){
            if(reach0.contains(p)) continue;
            reach0.insert(p);
            _BR_inner_br(bn, p, false, reach0, reach1);
        }
    }

    template<typename GUM_SCALAR>
    gum::NodeSet backdoor_reach(const gum::BayesNet<GUM_SCALAR>& bn, gum::NodeId a){
        const auto r = gum::Set({a});
        r += bn.parents(a);
        const auto l = gum::Set({a});
        for(const auto& pa : bn.parents(a)){
            _BR_inner_br(bn, pa, false, r, l);
        }
        auto s = r + l;
        if(s.contains(a)) s.erase(a);
        return s;
    }

    template<typename GUM_SCALAR>
    std::unique_ptr<NodeSet> _NOD_inner_nod(BayesNet<GUM_SCALAR> bn, NodeId a, NodeId b){
        if(a == b) return std::make_unique<NodeSet>();
        auto inners = std::make_unique<NodeSet>({a});
        auto chds = bn.children(a);
        if(chds.size() == 0) return nullptr;
        bool found = false;
        for(const auto& c : chds){
            auto s = _NOD_inner_nod(bn, c, b);
            if(s){
                found = true;
                *inners += *s;
            }
        }
        if(found) return std::move(inners);
        return nullptr;
    }

    template<typename GUM_SCALAR>
    std::unique_ptr<NodeSet> nodes_on_dipath(const BayesNet<GUM_SCALAR>& bn, NodeId x, NodeId y){
        auto r = _NOD_inner_nod(bn, x, y);
        if(r) r.remove(x);
        return std::move(r);
    }

    template<typename GUM_SCALAR>
    backdoor_iterator backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd){
        if(bn.parents(cause).size() == 0) return backdoor_iterator::end;
        if(bn.isParent(effect, cause)) return backdoor_iterator::end;

        // simplify the graph
        auto interest = NodeSet({cause, effect});
        auto G = std::make_shared<DAG>(dSep_reduce(bn, interest));

        {
            // removing the non connected in G without descendants
            // GG is a trash graph just to find the disjointed nodes in G
            auto GG = DiGraph(*G);
            for(const auto& i : bn.descendants(cause, {})) GG.eraseNode(i);

            // we only keep interesting connex components
            for(const auto& nodes : GG.connectedComponents().values()){
                if(!nodes.isdisjoint(interest)) continue;
                for(const auto& n : nodes) G->eraseNode(n);
            }
        }

        auto possible = G->nodes() - (bn.descendants(cause, {}) + interest + not_bd);
        if(possible.size() == 0) return backdoor_iterator::end;

        return backdoor_iterator(G, possible, cause, effect);
    }

    //   backdoors = set()

    //   for i in range(len(possible)):
    //     for subset in it.combinations(possible, i + 1):
    //       sub = frozenset(subset)
    //       worth_testing = True
    //       for s in backdoors:
    //         if s <= sub:
    //           worth_testing = False
    //       if worth_testing and isDSep_parents(G, {cause}, {effect}, sub):
    //         backdoors.add(sub)
    //         yield list(subset)



    // TODO
    // def frontdoor_generator(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId, not_fd: NodeSet = None):
    //   """
    //   Generates frontdoor sets for the pair of nodes `(x, y)` in the graph `bn` excluding the nodes in the set
    //   `not_fd` (optional)

    //   Parameters
    //   ----------
    //   bn: pyAgrum.BayesNet
    //   x: int
    //   y: int
    //   not_fd: Set[int] default=None

    //   Yields
    //   -------
    //   List[int]
    //     the different frontdoors
    //   """
    //   if isParent(x, y, bn):
    //     return

    //   if not_fd is None:
    //     not_fd = set()

    //   possible = nodes_on_dipath(bn, x, y)
    //   nodiPath = False
    //   if possible is None:
    //     nodiPath = True
    //     possible = set(bn.nodes()) - {x, y}
    //   possible -= backdoor_reach(bn, x)
    //   possible -= not_fd
    //   impossible = set()
    //   g = dSep_reduce(bn, {x, y} | possible)
    //   for z in possible:
    //     if backdoor_path(g, {z}, {y}, {x}):
    //       impossible.add(z)
    //   possible -= impossible
    //   frontdoors = set()

    //   if nodiPath:
    //     for s in possible:
    //       yield [s]
    //     return

    //   for i in range(len(possible)):
    //     for subset in it.combinations(possible, i + 1):
    //       sub = frozenset(subset)
    //       worth_testing = True
    //       for s in frontdoors:
    //         if s <= sub:
    //           worth_testing = False
    //       if worth_testing and not exists_unblocked_directed_path(bn, x, y, sub):
    //         frontdoors.add(sub)
    //         yield list(subset)

}