

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
     * In order to use this class, call backdoor_generator or frontdoor_generator.
     * 
     */
    class door_iterator { 
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = NodeSet;
        using pointer           = const value_type*;  // or also value_type*
        using reference         = const value_type&;

    protected:

        bool _is_the_end_;
        bool _is_front_door_;
        std::shared_ptr<DAG> _G_;
        std::shared_ptr<NodeSet> _possible_;
        NodeId _cause_;
        NodeId _effect_;
        Set<NodeSet> _doors_;
        std::vector<bool> _selection_mask_;
        size_t _selection_size_; //< inclusion mask for possible NodeSet
        value_type _cur_;

        friend class backdoor_iterator;
        template<typename GUM_SCALAR>
        friend class frontdoor_iterator;
        friend bool operator==(const door_iterator& a, const door_iterator& b);
        friend bool operator!=(const door_iterator& a, const door_iterator& b);
        friend constexpr door_iterator door_iterator_end(bool is_frontdoor);

        door_iterator(
            bool is_the_end,
            bool is_front_door,
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
            _is_front_door_(is_front_door),
            _G_(G),
            _possible_(possible),
            _cause_(cause),
            _effect_(effect),
            _doors_(backdoors),
            _selection_mask_(selection_mask),
            _selection_size_(selection_size),
            _cur_(cur)
        {}
        door_iterator(bool is_frontdoor)
            : door_iterator(true, is_frontdoor, nullptr, nullptr, 0, 0, Set<NodeSet>({}), std::vector<bool>({}), 0, NodeSet({}))
        {}

        bool _advance_selection_mask_();
        void _gen_cur_();

    public:
        /**
         * @brief Dereference operator 
         * 
         * @return reference 
        */
        reference operator*() const;
        /**
         * @brief Pointer operator
         * 
         * @return pointer 
         */
        pointer operator->() const;
    };

    class backdoor_iterator : public door_iterator {
    public:
        /**
         * @brief x++ operator for backdoor_iterator
         * @return backdoor_iterator& 
         */
        backdoor_iterator& operator++();

        /**
         * @brief WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         * @return backdoor_iterator 
         * @warning WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         */
        backdoor_iterator operator++(int);  // TODO: optimize this si j'ai la foie
        
        backdoor_iterator(backdoor_iterator&& v) = default;
        backdoor_iterator(backdoor_iterator& v) = default;
        backdoor_iterator& operator=(backdoor_iterator&& v);

        // template<typename GUM_SCALAR>
        // friend backdoor_iterable backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
        template<typename iter>
        friend class door_iterable;
    protected:
        using door_iterator::door_iterator;
        backdoor_iterator(const std::shared_ptr<DAG> G, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect);
        bool _next_();
    };
    static_assert(std::input_iterator<backdoor_iterator>);

    template<typename GUM_SCALAR>
    class frontdoor_iterator : public door_iterator {
    private:
        std::shared_ptr<BayesNet<GUM_SCALAR>> _bn_;
        bool _nodiPath_;
    public:
        /**
         * @brief x++ operator for frontdoor_iterator
         * @return frontdoor_iterator& 
         */
        frontdoor_iterator& operator++();

        /**
         * @brief WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         * @return frontdoor_iterator 
         * @warning WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         */
        frontdoor_iterator operator++(int);  // TODO: optimize this si j'ai la foie
        
        frontdoor_iterator(frontdoor_iterator<GUM_SCALAR>&& v) = default;
        frontdoor_iterator(frontdoor_iterator<GUM_SCALAR>& v) = default;
        frontdoor_iterator& operator=(frontdoor_iterator<GUM_SCALAR>&& v);

        // template<typename GUM_SCALAR>
        // friend frontdoor_iterable frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
        template<typename iter>
        friend class door_iterable;
    protected:
        using door_iterator::door_iterator;
        frontdoor_iterator(const std::shared_ptr<BayesNet<GUM_SCALAR>> bn, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect, bool nodiPath);
        bool _next_();
    };
    // static_assert(std::input_iterator<frontdoor_iterator>);

    template<typename iter>
    class door_iterable{ 
    private: 
        const iter _begin; 
        const iter _end; 
    protected: 
        inline door_iterable(iter&& begin, iter&& end) : _begin(begin), _end(end) {} 
        inline door_iterable() : _begin(!std::is_same<iter, backdoor_iterator>::value), _end(!std::is_same<iter, backdoor_iterator>::value) {}
    public: 
        inline iter begin() const { return _begin; }; 
        inline iter end() const { return _end; }; 

        template<typename GUM_SCALAR>
        friend door_iterable<backdoor_iterator> backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
        template<typename GUM_SCALAR>
        friend door_iterable<frontdoor_iterator<GUM_SCALAR>> frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
    };
    using backdoor_iterable = door_iterable<backdoor_iterator>;
    template<typename GUM_SCALAR>
    using frontdoor_iterable = door_iterable<frontdoor_iterator<GUM_SCALAR>>;

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
    backdoor_iterable backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd = NodeSet({}));
    
    /**
     * @brief Generates frontdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set `not_fd` (optional)
     * 
     * @tparam GUM_SCALAR 
     * @param bn 
     * @param cause 
     * @param effect 
     * @param not_fd 
     * @return backdoor_iterator 
     */
    template<typename GUM_SCALAR>
    frontdoor_iterable<GUM_SCALAR> frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_fd = NodeSet({}));
};

#include "doorCriteria_tpl.h"

#endif 