

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


    // TODO: check if combinations.hpp etc are needed

    /**
     * @brief Base iterator class for iterating over backdoors of a BayesNet
     * In order to use this class, call backdoor_generator or frontdoor_generator.
     * 
     */
    class DoorIterator { 
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

        friend class BackdoorIterator;
        template<typename GUM_SCALAR>
        friend class FrontdoorIterator;
        friend bool operator==(const DoorIterator& a, const DoorIterator& b);
        friend bool operator!=(const DoorIterator& a, const DoorIterator& b);

        DoorIterator(
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
        );
        DoorIterator(bool is_frontdoor);
        DoorIterator() = delete;

        bool _advance_selection_mask_();
        void _gen_cur_();

    public:
        ~DoorIterator();
        DoorIterator(DoorIterator&& v);
        DoorIterator(const DoorIterator& v);
        DoorIterator& operator=(DoorIterator&& v);
        DoorIterator& operator=(const DoorIterator& v);

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

    template<typename iter>
    class DoorIterable;

    class BackdoorIterator : public DoorIterator {
    public:
        /**
         * @brief x++ operator for BackdoorIterator
         * @return BackdoorIterator& 
         */
        BackdoorIterator& operator++();

        /**
         * @brief WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         * @return BackdoorIterator 
         * @warning WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         */
        BackdoorIterator operator++(int);  // TODO: optimize this si j'ai la foie
        
        BackdoorIterator(BackdoorIterator&& v);
        BackdoorIterator(const BackdoorIterator& v);
        ~BackdoorIterator();
        BackdoorIterator& operator=(BackdoorIterator&& v);
        BackdoorIterator& operator=(const BackdoorIterator& v);

        template<typename GUM_SCALAR>
        friend DoorIterable<BackdoorIterator> backdoor_generator(const BayesNet<GUM_SCALAR>&, NodeId, NodeId, const NodeSet&);
        template<typename iter>
        friend class DoorIterable;
    protected:
        BackdoorIterator();
        BackdoorIterator(const std::shared_ptr<DAG> G, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect);
        bool _next_();
    };
    static_assert(std::input_iterator<BackdoorIterator>);

    template<typename GUM_SCALAR>
    class FrontdoorIterator : public DoorIterator {
    private:
        std::shared_ptr<BayesNet<GUM_SCALAR>> _bn_;
        bool _nodiPath_;
    public:
        /**
         * @brief x++ operator for FrontdoorIterator
         * @return FrontdoorIterator& 
         */
        FrontdoorIterator& operator++();

        /**
         * @brief WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         * @return FrontdoorIterator 
         * @warning WARNING, performs a complete copy of this structure! Potentially slow, always prefer ++x operator!!! 
         */
        FrontdoorIterator operator++(int);  // TODO: optimize this si j'ai la foie
        
        FrontdoorIterator(FrontdoorIterator<GUM_SCALAR>&& v);
        FrontdoorIterator(const FrontdoorIterator<GUM_SCALAR>& v);
        ~FrontdoorIterator();
        FrontdoorIterator<GUM_SCALAR>& operator=(FrontdoorIterator<GUM_SCALAR>&& v);
        FrontdoorIterator<GUM_SCALAR>& operator=(const FrontdoorIterator<GUM_SCALAR>& v);


        template<typename GUM_SCALAR>
        friend DoorIterable<FrontdoorIterator<GUM_SCALAR>> frontdoor_generator(const BayesNet<GUM_SCALAR>&, NodeId, NodeId, const NodeSet&);
        template<typename iter>
        friend class DoorIterable;
    protected:
        FrontdoorIterator();
        FrontdoorIterator(const std::shared_ptr<BayesNet<GUM_SCALAR>> bn, const std::shared_ptr<NodeSet> possible, NodeId cause, NodeId effect, bool nodiPath);
        bool _next_();
    };
    // static_assert(std::input_iterator<FrontdoorIterator>);

    template<typename iter>
    class DoorIterable{ 
    private: 
        const iter _begin; 
        const iter _end; 
    protected: 
        INLINE DoorIterable(iter&& begin, iter&& end); 
        INLINE DoorIterable();

    public: 
        INLINE ~DoorIterable();
        INLINE DoorIterable(DoorIterable<iter>&& v);
        INLINE DoorIterable(const FrontdoorIterator<iter>& v);
        INLINE DoorIterable<iter>& operator=(DoorIterable<iter>&& v);
        INLINE DoorIterable<iter>& operator=(const DoorIterable<iter>& v);
        
        INLINE iter begin() const; 
        INLINE iter end() const; 

        template<typename GUM_SCALAR>
        friend DoorIterable<BackdoorIterator> backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
        template<typename GUM_SCALAR>
        friend DoorIterable<FrontdoorIterator<GUM_SCALAR>> frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd);
    };
    using BackdoorIterable = DoorIterable<BackdoorIterator>;
    template<typename GUM_SCALAR>
    using FrontdoorIterable = DoorIterable<FrontdoorIterator<GUM_SCALAR>>;

    /**
     * @brief Generates backdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set `not_bd` (optional)
     * 
     * @tparam GUM_SCALAR 
     * @param bn 
     * @param cause 
     * @param effect 
     * @param not_bd 
     * @return BackdoorIterator 
     */
    template<typename GUM_SCALAR>
    BackdoorIterable backdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_bd = NodeSet({}));
    
    /**
     * @brief Generates frontdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set `not_fd` (optional)
     * 
     * @tparam GUM_SCALAR 
     * @param bn 
     * @param cause 
     * @param effect 
     * @param not_fd 
     * @return BackdoorIterator 
     */
    template<typename GUM_SCALAR>
    FrontdoorIterable<GUM_SCALAR> frontdoor_generator(const BayesNet<GUM_SCALAR>& bn, NodeId cause, NodeId effect, const NodeSet& not_fd = NodeSet({}));
};

#include "doorCriteria_tpl.h"

#ifndef GUM_NO_INLINE
#include "doorCriteria_inl.h"
#endif

#endif 