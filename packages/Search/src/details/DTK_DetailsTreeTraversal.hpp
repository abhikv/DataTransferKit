/****************************************************************************
 * Copyright (c) 2012-2018 by the DataTransferKit authors                   *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the DataTransferKit library. DataTransferKit is     *
 * distributed under a BSD 3-clause license. For the licensing terms see    *
 * the LICENSE file in the top-level directory.                             *
 *                                                                          *
 * SPDX-License-Identifier: BSD-3-Clause                                    *
 ****************************************************************************/
#ifndef DTK_DETAILS_TREE_TRAVERSAL_HPP
#define DTK_DETAILS_TREE_TRAVERSAL_HPP

#include <DTK_DBC.hpp>

#include <DTK_DetailsAlgorithms.hpp>
#include <DTK_DetailsNode.hpp>
#include <DTK_DetailsPriorityQueue.hpp>
#include <DTK_DetailsStack.hpp>
#include <DTK_Predicates.hpp>

namespace DataTransferKit
{

template <typename DeviceType>
class BoundingVolumeHierarchy;

namespace Details
{
template <typename DeviceType>
struct TreeTraversal
{
  public:
    using ExecutionSpace = typename DeviceType::execution_space;

    template <typename Predicate, typename... Args>
    KOKKOS_INLINE_FUNCTION static int
    query( BoundingVolumeHierarchy<DeviceType> const &bvh,
           Predicate const &pred, Args &&... args )
    {
        using Tag = typename Predicate::Tag;
        return queryDispatch( Tag{}, bvh, pred, std::forward<Args>( args )... );
    }

    /**
     * Return true if the node is a leaf.
     */
    KOKKOS_INLINE_FUNCTION
    static bool isLeaf( Node const *node )
    {
        return ( node->children.first == nullptr );
    }

    /**
     * Return the index of the leaf node.
     */
    KOKKOS_INLINE_FUNCTION
    static size_t getIndex( Node const *leaf )
    {
        static_assert( sizeof( size_t ) == sizeof( Node * ),
                       "Conversion is a bad idea if these sizes do not match" );
        return reinterpret_cast<size_t>( leaf->children.second );
    }

    /**
     * Return the root node of the BVH.
     */
    KOKKOS_INLINE_FUNCTION
    static Node const *getRoot( BoundingVolumeHierarchy<DeviceType> const &bvh )
    {
        if ( bvh.empty() )
            return nullptr;
        return ( bvh.size() > 1 ? bvh._internal_nodes : bvh._leaf_nodes )
            .data();
    }
};

// There are two (related) families of search: one using a spatial predicate and
// one using nearest neighbours query (see boost::geometry::queries
// documentation).
template <typename DeviceType, typename Predicate, typename Insert>
KOKKOS_FUNCTION int
spatialQuery( BoundingVolumeHierarchy<DeviceType> const &bvh,
              Predicate const &predicate, Insert const &insert )
{
    if ( bvh.empty() )
        return 0;

    if ( bvh.size() == 1 )
    {
        Node const *leaf = TreeTraversal<DeviceType>::getRoot( bvh );
        if ( predicate( leaf ) )
        {
            int const leaf_index = TreeTraversal<DeviceType>::getIndex( leaf );
            insert( leaf_index );
            return 1;
        }
        else
            return 0;
    }

    Stack<Node const *> stack;

    stack.emplace( TreeTraversal<DeviceType>::getRoot( bvh ) );
    int count = 0;

    while ( !stack.empty() )
    {
        Node const *node = stack.top();
        stack.pop();

        if ( TreeTraversal<DeviceType>::isLeaf( node ) )
        {
            insert( TreeTraversal<DeviceType>::getIndex( node ) );
            count++;
        }
        else
        {
            for ( Node const *child :
                  {node->children.first, node->children.second} )
            {
                if ( predicate( child ) )
                {
                    stack.push( child );
                }
            }
        }
    }
    return count;
}

// query k nearest neighbours
template <typename DeviceType, typename Distance, typename Insert,
          typename Buffer>
KOKKOS_FUNCTION int
nearestQuery( BoundingVolumeHierarchy<DeviceType> const &bvh,
              Distance const &distance, std::size_t k, Insert const &insert,
              Buffer const &buffer )
{
    if ( bvh.empty() || k < 1 )
        return 0;

    if ( bvh.size() == 1 )
    {
        Node const *leaf = TreeTraversal<DeviceType>::getRoot( bvh );
        int const leaf_index = TreeTraversal<DeviceType>::getIndex( leaf );
        double const leaf_distance = distance( leaf );
        insert( leaf_index, leaf_distance );
        return 1;
    }

    // Nodes with a distance that exceed that radius can safely be discarded.
    // Initialize the radius to infinity and tighten it once k neighbors have
    // been found.
    double radius = KokkosHelpers::ArithTraits<double>::infinity();

    using PairIndexDistance = Kokkos::pair<int, double>;
    static_assert(
        std::is_same<typename Buffer::value_type, PairIndexDistance>::value,
        "Type of the elements stored in the buffer passed as argument to "
        "TreeTraversal::nearestQuery is not right" );
    struct CompareDistance
    {
        KOKKOS_INLINE_FUNCTION bool
        operator()( PairIndexDistance const &lhs,
                    PairIndexDistance const &rhs ) const
        {
            return lhs.second < rhs.second;
        }
    };
    // Use a priority queue for convenience to store the results and preserve
    // the heap structure internally at all time.  There is no memory
    // allocation, elements are stored in the buffer passed as an argument.
    // The farthest leaf node is on top.
    assert( k == buffer.size() );
    PriorityQueue<PairIndexDistance, CompareDistance,
                  UnmanagedStaticVector<PairIndexDistance>>
        heap( UnmanagedStaticVector<PairIndexDistance>( buffer.data(),
                                                        buffer.size() ) );

    using PairNodePtrDistance = Kokkos::pair<Node const *, double>;
    Stack<PairNodePtrDistance> stack;
    // Do not bother computing the distance to the root node since it is
    // immediately popped out of the stack and processed.
    stack.emplace( TreeTraversal<DeviceType>::getRoot( bvh ), 0. );

    while ( !stack.empty() )
    {
        Node const *node = stack.top().first;
        double const node_distance = stack.top().second;
        stack.pop();

        if ( node_distance < radius )
        {
            if ( TreeTraversal<DeviceType>::isLeaf( node ) )
            {
                int const leaf_index =
                    TreeTraversal<DeviceType>::getIndex( node );
                double const leaf_distance = node_distance;
                if ( heap.size() < k )
                {
                    // Insert leaf node and update radius if it was the kth one.
                    heap.push( Kokkos::make_pair( leaf_index, leaf_distance ) );
                    if ( heap.size() == k )
                        radius = heap.top().second;
                }
                else
                {
                    // Replace top element in the heap and update radius.
                    heap.popPush(
                        Kokkos::make_pair( leaf_index, leaf_distance ) );
                    radius = heap.top().second;
                }
            }
            else
            {
                // Insert children into the stack and make sure that the
                // closest one ends on top.
                Node const *left_child = node->children.first;
                double const left_child_distance = distance( left_child );
                Node const *right_child = node->children.second;
                double const right_child_distance = distance( right_child );
                if ( left_child_distance < right_child_distance )
                {
                    // NOTE not really sure why but it performed better with
                    // the conditional insertion on the device and without it
                    // on the host (~5% improvement for both)
#if defined( __CUDA_ARCH__ )
                    if ( right_child_distance < radius )
#endif
                        stack.emplace( right_child, right_child_distance );
                    stack.emplace( left_child, left_child_distance );
                }
                else
                {
#if defined( __CUDA_ARCH__ )
                    if ( left_child_distance < radius )
#endif
                        stack.emplace( left_child, left_child_distance );
                    stack.emplace( right_child, right_child_distance );
                }
            }
        }
    }
    // Sort the leaf nodes and output the results.
    // NOTE: Do not try this at home.  Messing with the underlying container
    // invalidates the state of the PriorityQueue.
    sortHeap( heap.data(), heap.data() + heap.size(), heap.valueComp() );
    for ( decltype( heap.size() ) i = 0; i < heap.size(); ++i )
    {
        int const leaf_index = ( heap.data() + i )->first;
        double const leaf_distance = ( heap.data() + i )->second;
        insert( leaf_index, leaf_distance );
    }
    return heap.size();
}

template <typename DeviceType, typename Predicate, typename Insert>
KOKKOS_INLINE_FUNCTION int
queryDispatch( SpatialPredicateTag,
               BoundingVolumeHierarchy<DeviceType> const &bvh,
               Predicate const &pred, Insert const &insert )
{
    return spatialQuery( bvh, pred, insert );
}

template <typename DeviceType, typename Predicate, typename Insert,
          typename Buffer>
KOKKOS_INLINE_FUNCTION int queryDispatch(
    NearestPredicateTag, BoundingVolumeHierarchy<DeviceType> const &bvh,
    Predicate const &pred, Insert const &insert, Buffer const &buffer )
{
    auto const geometry = pred._geometry;
    auto const k = pred._k;
    return nearestQuery( bvh,
                         [geometry]( Node const *node ) {
                             return distance( geometry, node->bounding_box );
                         },
                         k, insert, buffer );
}

} // namespace Details
} // namespace DataTransferKit

#endif
