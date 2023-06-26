

#ifndef GUM_DOOR_CRITERIA_H
#define GUM_DOOR_CRITERIA_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/set.h>
#include <string>
#include <iterator>

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
    std::unique_ptr<NodeSet> nodes_on_dipath(const BayesNet<GUM_SCALAR>& bn, NodeId x, NodeId y);


    struct backdoor_iterator_sentinel {};
    struct backdoor_iterator {
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = NodeSet;
        using pointer           = const value_type*;  // or also value_type*
        using reference         = const value_type&;

        static constexpr backdoor_iterator_sentinel end{};

        backdoor_iterator(backdoor_iterator&& v);
        backdoor_iterator(backdoor_iterator& v);

        reference operator*() const;
        pointer operator->() const;
        backdoor_iterator& operator++();
        backdoor_iterator operator++(int);
        backdoor_iterator& operator=(backdoor_iterator&& v);
        friend bool operator==(const backdoor_iterator& a, const backdoor_iterator& b);
        friend bool operator!=(const backdoor_iterator& a, const backdoor_iterator& b);
        friend bool operator==(const backdoor_iterator& a, backdoor_iterator_sentinel);
        friend bool operator!=(const backdoor_iterator& a, backdoor_iterator_sentinel);
        template<typename GUM_SCALAR>
        friend backdoor_iterator backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);

    private:
        std::shared_ptr<DAG> _G_;
        std::shared_ptr<Set<NodeSet>> _backdoors_;
        std::shared_ptr<NodeSet> _possible_;
        NodeId _cause_;
        NodeId _effect_;
        std::shared_ptr<std::vector<bool>> _v_;
        std::shared_ptr<NodeSet> _cur_;

        backdoor_iterator(std::shared_ptr<DAG> G, std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect);
        void _next_();
        void _comb_(size_t r);
    };
    static_assert(std::input_iterator<backdoor_iterator>);

    /**
     * @brief Generates backdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set `not_bd` (optional)
     * 
     * @tparam GUM_SCALAR 
     * @param bn 
     * @param cause 
     * @param effect 
     * @param not_bd 
     * @return backdoor_iterator 
     */
    template<typename GUM_SCALAR>
    backdoor_iterator backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd = {});

};

#include "doorCriteria_tpl.h"

#endif 