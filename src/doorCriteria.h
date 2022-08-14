

#ifndef GUM_DOOR_CRITERIA_H
#define GUM_DOOR_CRITERIA_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/set.h>
#include <string>

namespace gum{
    /**
     * @brief  Predicate on the existence of an open back door path 
     * from ``x`` to ``y``, conditioning on the set of variables ``zset``
     * 
     * @tparam GUM_SCALAR 
     * @param bn the DAG model
     * @param x name of source node 
     * @param y name of destination node
     * @param zset names of conditioning nodes
     * @return true (it exists)
     * @return false 
     */
    template<typename GUM_SCALAR>
    bool backdoor_path(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset);


    /**
     * @brief Predicate on the existence of a directed path from 
     * ``x`` to ``y`` in the Bayesian network ``bn`` not blocked by nodes 
     * of ``zset``
     * 
     * @tparam GUM_SCALAR 
     * @param bn the DAG model
     * @param x name of source node
     * @param y name of destination node
     * @param zset names of the conditioning nodes
     * @return true 
     * @return false 
     */
    template<typename GUM_SCALAR>
    bool exists_unblocked_directed_path(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y,
        const gum::Set<std::string>& zset);

    /**
     * @brief Tests whether or not ``zset`` satisifies the front 
     * door criterion for ``x`` and ``y``, in the Bayesian network ``bn``
     * 
     * @tparam GUM_SCALAR 
     * @param bn the DAG model
     * @param x name of source node
     * @param y name of destination node
     * @param zset names of the conditioning nodes
     * @return true 
     * @return false 
     */
    template<typename GUM_SCALAR>
    bool is_frontdoor(
        const gum::BayesNet<GUM_SCALAR>& bn, 
        const std::string& x, 
        const std::string& y, 
        const gum::Set<std::string>& zset);

    /**
     * @brief  Tests whether or not ``zset`` satisifies the back door criterion for ``x`` and ``y``, in the Bayesian network ``bn``
     * 
     * @tparam GUM_SCALAR 
     * @param bn the DAG model
     * @param x name of source node
     * @param y name of destination node
     * @param zset names of conditioning nodes
     * @return true 
     * @return false 
     */
    template<typename GUM_SCALAR>
    bool is_backdoor(const gum::BayesNet<GUM_SCALAR>& bn, const std::string& x, const std::string& y, const gum::Set<std::string>& zset);

    /**
     * @brief Returns the set of nodes that can be reached through a
     * backdoor path from ``a`` in the graph ``bn``
     * 
     * @tparam GUM_SCALAR 
     * @param bn the DAG model
     * @param a the backdoor node
     * @return gum::NodeSet 
     */
    template<typename GUM_SCALAR>
    gum::NodeSet backdoor_reach(const gum::BayesNet<GUM_SCALAR>& bn, gum::NodeId a);

    /**
     * @brief Returns the set of nodes through which there is a
     * directed path from `x` to `y` in the graph `bn`
     * 
     * @tparam UM_SCALAR 
     * @param bn 
     * @param x 
     * @param y 
     * @return const gum::NodeSet* 
     */
    template<typename GUM_SCALAR>
    const gum::NodeSet* nodes_on_dipath(const BayesNet<GUM_SCALAR>& bn, NodeId x, NodeId y);
    // TODO:
    //   def inner_nod(g: "pyAgrum.BayesNet", a: NodeId, b: NodeId) -> Optional[NodeSet]:
    //     if b == a:
    //       return set()

    //     inners = {a}
    //     children = g.children(a)
    //     if len(children) == 0:
    //       return None

    //     found = False
    //     for c in children:
    //       s = inner_nod(g, c, b)
    //       if s is not None:
    //         found = True
    //         inners |= s
    //     if found:
    //       return inners
    //     return None

    //   r = inner_nod(bn, x, y)
    //   if r:
    //     r.remove(x)
    //   return r



    // std::vector<gum::NodeId> 

    // def backdoor_generator(bn: "pyAgrum.BayesNet", cause: NodeId, effect: NodeId, not_bd: NodeSet = None):
    //   """
    //   Generates backdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set
    //   `not_bd` (optional)

    //   Parameters
    //   ----------
    //   bn: pyAgrum.BayesNet
    //   cause: int
    //   effect: int
    //   not_bd: Set[int] default=None

    //   Yields
    //   -------
    //   List[int]
    //     the different backdoors
    //   """
    //   if len(bn.parents(cause)) == 0:  # no parent of cause, no backdoor
    //     return
    //   if isParent(effect, cause, bn):  # causalDagFromBN(bn)):
    //     return

    //   if not_bd is None:
    //     not_bd = set()

    //   # simplifying the graph
    //   interest = {cause, effect}
    //   G = dSep_reduce(bn, interest)

    //   # removing the non connected in G without descendants
    //   # GG is a trash graph just to find the disjointed nodes in G
    //   GG = pyAgrum.DiGraph(G)
    //   for i in descendants(bn, cause, set()):
    //     GG.eraseNode(i)

    //   # we only keep interesting connex components
    //   for nodes in GG.connectedComponents().values():
    //     if nodes.isdisjoint(interest):
    //       for n in nodes:
    //         G.eraseNode(n)

    //   possible = set(G.nodes()) - (descendants(bn, cause, set()) | interest | not_bd)
    //   if len(possible) == 0:
    //     return

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

    //     def frontdoor_generator(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId, not_fd: NodeSet = None):
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

#include "doorCriteria_tpl.h"

#endif 