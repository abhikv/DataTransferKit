/****************************************************************************
 * Copyright (c) 2012-2018 by the DataTransferKit authors                   *
 * All rights reserved.                                                     *
 *                                                                          *
 * This file is part of the DataTransferKit library. DataTransferKit is     *
 * distributed under a BSD 3-clause license. For the licensing terms see    *
 * the LICENSE file in the top-level directory.                             *
 ****************************************************************************/
/*!
 * \file
 * \brief DTK hardcoded types.
 */
#ifndef DTK_TYPES_H
#define DTK_TYPES_H

#ifdef __cplusplus
#include <cstdint>
#else
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <stdint.h>
#endif
#endif

//! Coordinate typedef.
typedef double Coordinate;

//! Local ordinal typedef.
typedef unsigned int LocalOrdinal;

//! Global ordinal typedef.
typedef uint64_t GlobalOrdinal;

#endif // DTK_TYPES_H
