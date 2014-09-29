//---------------------------------------------------------------------------//
/*
  Copyright (c) 2012, Stuart R. Slattery
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  *: Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  *: Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  *: Neither the name of the University of Wisconsin - Madison nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//---------------------------------------------------------------------------//
/*!
 * \brief DTK_GeometricEntity.hpp
 * \author Stuart R. Slattery
 * \brief Geometric entity interface.
 */
//---------------------------------------------------------------------------//

#ifndef DTK_GEOMETRICENTITY_HPP
#define DTK_GEOMETRICENTITY_HPP

#include "DTK_MeshTypes.hpp"
#include "DTK_BoundingBox.hpp"

#include <Teuchos_ArrayView.hpp>

namespace DataTransferKit
{
//---------------------------------------------------------------------------//
/*!
  \class GeometricEntity
  \brief Geometry traits definitions.

  GeometricEntity provide access to basic properites of geometric objects. A
  geometry is simply an object or collection of objects that has $n$ physical
  dimensions and a spatial domain $\Omega \in \mathbb{R}^n$ that is bounded by
  a boundary $\Gamma \in \mathbb{R}^n$. Concrete examples of geometries in 3
  dimensions include cubes and cylinders. A geometry can have 1, 2, or three
  dimensions. To specify the general position in space of the geometry, each
  object is required to have a centroid given in Cartesian coordinates with
  (x) given for 1 dimensional geometries, (x,y) for two dimensions, and
  (x,y,z) for 3 dimensions. A measure is also specified for each geometry
  where the measure is defined as length in 1 dimension, area in 2 dimensions,
  and volume for 3 dimensions. In addition to this data, a geometry must be
  able to provide a Cartesian axis-aligned bounding box that encapsulates the
  entire geometry. For geometric search operations to be performed, a geometry
  must be able to determine if a given point of the same dimensionality as the
  geometry is contained within the boundary of the geometry (i.e. $\hat{r} \in
  \Omega$).
*/
//---------------------------------------------------------------------------//
class GeometricEntity
{
  public:

    /*!
     * \brief Constructor.
     */
    GeometricEntity()
    { /* ... */ }

    /*!
     * \brief Destructor.
     */
    virtual ~GeometricEntity()
    { /* ... */ }

    /*!
     * \brief Return the dimension of the geometry.
     */
    virtual int dimension() const = 0;

    /*!
     * \brief Return the geometry measure (volume for a 3D geometry, area for
     * 2D, and length for 1D).
     */
    virtual double measure() const = 0;

    /*!
     * \brief Return the centroid of the geometry.
     */
    virtual Teuchos::Array<double> centroid() const = 0;

    /*!
     * \brief Return the axis-aligned bounding box around the geometry.
     */
    virtual BoundingBox boundingBox() const = 0;

    /*!
     * \brief Return whether or not a point is in the entity within a
     * specified tolerance.
     */
    virtual bool pointInEntity( const Teuchos::ArrayView<double>& coords,
				const double tolerance ) const = 0;
};

//---------------------------------------------------------------------------//

} // end namespace DataTransferKit

#endif // end DTK_GEOMETRICENTITY_HPP

//---------------------------------------------------------------------------//
// end DTK_GeometricEntity.hpp
//---------------------------------------------------------------------------//
