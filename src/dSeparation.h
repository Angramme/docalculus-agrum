

#ifndef GUM_DSEPARATION_H
#define GUM_DSEPARATION_H

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/undiGraph.h>
#include "CausalModel.h"


namespace gum{


    /**
     * @brief Predicate on whether ``a`` is parent of ``b`` in the graph ``g``, the graph must be a DAG 
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param a 
     * @param b 
     * @param bn 
     * @return true 
     * @return false 
     */
    template<typename GraphT>
    bool isParent(NodeId a, NodeId b, const GraphT& bn);



    /**
     * @brief Returns the undirected graph obtained by reducing (ancestor graph) and moralizing the Bayesian network ``bn``
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn the BayesNet
     * @param x NodeSet generating the ancestor graph
     * @param y Second NodeSet generating the ancestor graph
     * @param zset Third NodeSet generating the ancestor graph
     * @return UndiGraph The reduced moralized graph
     */
    template<typename GraphT>
    UndiGraph reduce_moralize(const GraphT& bn, const NodeSet& x, const NodeSet& y, const NodeSet& zset);



    /**
     * @brief Remove all nodes in zset from the graph.
     * 
     * @param gg undirected graph
     * @param zset nodes to remove
     */
    void remove_nodes(UndiGraph& gg, const NodeSet& zset);




    /**
     * @brief Predicate asserting the existence of a path between 
     * ``x`` and ``y`` in the non-oriented graph``g_undi``, without 
     * going through the optional marking set ``mark``
     * 
     * @param gg The graph
     * @param sx first node 
     * @param sy second node
     * @param marked forbidden nodes
     * @return true 
     * @return false 
     */
    bool is_path_x_y(const UndiGraph& gg, const NodeSet& sx, const NodeSet& sy, const NodeSet& marked = NodeSet({}));



    /**
     * @brief Test of d-separation for ``x`` and ``y``, given ``zset`` using the graph-moralization method
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn the bayesian network
     * @param sx source nodes
     * @param sy destinantion nodes
     * @param zset blocking set
     * @return true if ``Z`` d-separates ``x`` and ``y``
     * @return false 
     */
    template<typename GraphT>
    bool isDSep(const GraphT& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset);



    /**
     * @brief Test of d-separation of ``sx`` and ``sy`` given ``Z``, 
     * considering only the paths with an arc coming into ``x`` using 
     * the graph-moralization method
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn the bayesian network
     * @param sx source nodes
     * @param sy destinantion nodes
     * @param zset blocking set
     * @return true if ``Z`` d-separates ``x`` and ``y``
     * @return false 
     */
    template<typename GraphT>
    bool isDSep_parents(const GraphT& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset);



    /**
     * @brief Test of d-separation of ``x`` and ``y`` given ``zset``, 
     * considering only the paths with an arc coming from ``x`` using 
     * the graph-moralization method
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn the bayesian network
     * @param sx source nodes
     * @param sy destinantion nodes
     * @param zset blocking set
     * @return true 
     * @return false 
     */
    template<typename GraphT>
    bool isDSep_tech2_children(const GraphT& bn, const NodeSet& sx, const NodeSet& sy, const NodeSet& zset);



    /**
     * @brief Asserts whether or not ``x`` is a descendant of ``y`` in ``bn``
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn 
     * @param x 
     * @param y 
     * @param marked 
     * @return true 
     * @return false 
     */
    template<typename GraphT>
    bool is_descendant(const GraphT& bn, NodeId x, NodeId y, const NodeSet& marked);

    /**
     * @brief Returns the set of recursively determined barren nodes in 
     * ``bn`` relatively to the set of nodes ``interest`` (if ``interest`` 
     * is void, then the whole set of nodes in the graph will be returned)
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn 
     * @param interest 
     * @return const NodeSet& 
     */
    template<typename GraphT>
    NodeSet barren_nodes(const GraphT& bn, const NodeSet& interest);



    /**
     * @brief Creates and returns a duplicate DAG of the given 
     * Bayesian network, excluding the nodes provided in the nexcl set.
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param bn 
     * @param nexcl 
     * @return DAG 
     */
    template<typename GraphT>
    DAG partialDAGfromBN(const GraphT& bn, const NodeSet& nexcl = {});


    /**
     * @brief Reduce a BN by removing barren nodes w.r.t a set of nodes.
     * 
     * @tparam GraphT structure implementing a DAG-like interface
     * @param g the source
     * @param interest the nodes of interest
     * @return DAG the reduced DAG
     */
    template<typename GraphT>
    DAG dSep_reduce(const GraphT& g, const NodeSet& interest);


    /**
     * @brief Adds the ancestors of ``x`` in the Directed Model ``dm`` to the set ``anc``
     * 
     * @tparam DirectedModel : BayesNet or DAG or CausalModel
     * @param x 
     * @param dm 
     * @param anc 
     */
    template<typename DirectedModel>
    void ancestor(NodeId x, DirectedModel& dm, NodeSet& anc);

    /**
     * @brief Returns a set composed by all the descendents of ``x`` in ``bn``
     * 
     * @tparam GUM_SCALAR 
     * @param bn 
     * @param x 
     * @param marked 
     * @return NodeSet 
     */
    template<typename GUM_SCALAR>
    NodeSet descendants(const BayesNet<GUM_SCALAR>& bn, NodeId x, const NodeSet& marked = NodeSet({}));
}

#include "dSeparation_tpl.h"

#endif GUM_DSEPARATION_H