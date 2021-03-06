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

#ifndef DTK_NEAREST_NEIGHBOR_OPERATOR_DECL_HPP
#define DTK_NEAREST_NEIGHBOR_OPERATOR_DECL_HPP

#include <DTK_PointCloudOperator.hpp>

namespace DataTransferKit
{

template <typename DeviceType>
class NearestNeighborOperator : public PointCloudOperator<DeviceType>
{
    using ExecutionSpace = typename DeviceType::execution_space;

  public:
    NearestNeighborOperator(
        Teuchos::RCP<Teuchos::Comm<int> const> const &comm,
        Kokkos::View<Coordinate const **, DeviceType> source_points,
        Kokkos::View<Coordinate const **, DeviceType> target_points );

    void
    apply( Kokkos::View<double const *, DeviceType> source_values,
           Kokkos::View<double *, DeviceType> target_values ) const override;

  private:
    Teuchos::RCP<Teuchos::Comm<int> const> _comm;
    Kokkos::View<int *, DeviceType> _indices;
    Kokkos::View<int *, DeviceType> _ranks;
    int const _size;
};

} // namespace DataTransferKit

#endif
