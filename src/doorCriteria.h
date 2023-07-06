

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


    /**
     * @brief Base iterator class for iterating over backdoors of a BayesNet
     * In order to use this class, call backdoor_generator function.
     * 
     */
    struct backdoor_iterator_base { //  TODO: docs
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = NodeSet;
        using pointer           = const value_type*;  // or also value_type*
        using reference         = const value_type&;

    protected:
        bool _is_the_end_;
        std::shared_ptr<DAG> _G_;
        std::shared_ptr<NodeSet> _possible_;
        NodeId _cause_;
        NodeId _effect_;
        Set<NodeSet> _backdoors_;
        std::vector<bool> _selection_mask_;
        size_t _selection_size_; //< inclusion mask for possible NodeSet
        value_type _cur_;

        friend struct backdoor_iterator;
        friend bool operator==(const backdoor_iterator_base& a, const backdoor_iterator_base& b);
        friend bool operator!=(const backdoor_iterator_base& a, const backdoor_iterator_base& b);

        backdoor_iterator_base(
            bool is_the_end,
            std::shared_ptr<DAG> G,
            std::shared_ptr<NodeSet> possible,
            NodeId cause,
            NodeId effect,
            Set<NodeSet> backdoors,
            std::vector<bool> selection_mask,
            size_t selection_size,
            value_type cur
        ) :
            _is_the_end_(is_the_end),
            _G_(G),
            _possible_(possible),
            _cause_(cause),
            _effect_(effect),
            _backdoors_(backdoors),
            _selection_mask_(selection_mask),
            _selection_size_(selection_size),
            _cur_(cur)
        {}
    };
    struct backdoor_iterator : public backdoor_iterator_base { // TODO: docs
    private:
        inline static const backdoor_iterator_base _get_end(){
            return backdoor_iterator_base::backdoor_iterator_base( true, nullptr, nullptr, 0, 0, Set<NodeSet>({}), std::vector<bool>({}), 0, NodeSet({}) );
        }
    public:
        inline static const backdoor_iterator_base end = _get_end();

        reference operator*() const;
        pointer operator->() const;
        
        /**
         * @brief x++ operator for backdoor_iterator
         * 
         * @return backdoor_iterator& 
         */
        backdoor_iterator& operator++();

        /**
         * @brief WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         * 
         * @return backdoor_iterator 
         */
        backdoor_iterator operator++(int);  // TODO: optimize this
        
        backdoor_iterator(backdoor_iterator&& v) = default;
        backdoor_iterator(backdoor_iterator& v) = default;
        backdoor_iterator& operator=(backdoor_iterator&& v);


        template<typename GUM_SCALAR>
        friend backdoor_iterator backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
    protected:
        backdoor_iterator(const std::shared_ptr<DAG> G, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect);
        bool _next_();
        bool _advance_selection_mask_();
        void _gen_cur_();
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


    // TODO: add frontdoor generator and iterator, make a base class and derive
    // replace the error-prone logic with the use of cppitertools/combinations iterator
};

#include "doorCriteria_tpl.h"

#endif 