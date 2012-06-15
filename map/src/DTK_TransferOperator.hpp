//---------------------------------------------------------------------------//
/*!
 * \file DTK_TransferOperator.hpp
 * \author Stuart R. Slattery
 * \brief Transfer operator declaration.
 */
//---------------------------------------------------------------------------//

#ifndef DTK_TRANSFEROPERATOR_HPP
#define DTK_TRANSFEROPERATOR_HPP

#include <Teuchos_RCP.hpp>

namespace DataTransferKit
{

template<class Map>
class TransferOperator
{
  public:

    //@{
    //! Typedefs.
    typedef Map                  map_type;
    typedef Teuchos::RCP<Map>    RCP_Map;

    //! Constructor.
    TransferOperator( const RCP_Map& map )
	: d_map( map )
    { /* ... */ }

    //! Destructor.
    ~TransferOperator()
    { /* ... */ }

    // Transfer operator setup.
    template<class SourceGeometry, class TargetGeometry>
    inline void setup( const SourceGeometry& source_geometry,
		       const TargetGeometry& target_geometry );


    // Transfer operator apply.
    template<class SourceField, class TargetField>
    inline void apply( const SourceField& source_field, 
		       TargetField& target_field );

  private:

    // Map.
    RCP_Map d_map;
};

//---------------------------------------------------------------------------//
// Inline functions.
//---------------------------------------------------------------------------//
/*!
 * \brief Transfer operator setup.
 * \param source_geometry The source geometry for the transfer operation.
 * \param target_geometry The target geometry for the transfer operation.
 */
template<class SourceGeometry, class TargetGeometry, class Map>
void TransferOperator::setup( const SourceGeometry& source_geometry, 
			      TargetGeometry& target_geometry )
{
    d_map->setup( source_geometry, target_geometry );
}
 
//---------------------------------------------------------------------------//
/*!
 * \brief Transfer operator apply.
 * \param source_field The source field for the transfer operation.
 * \param target_field The target field for the transfer operation.
 */
template<class SourceField, class TargetField, class Map>
void TransferOperator::apply( const SourceField& source_field, 
			      TargetField& target_field )
{
    d_map->apply( source_field, target_field );
}
 
//---------------------------------------------------------------------------//

} // end namespace DataTransferKit

#endif // DTK_TRANSFEROPERATOR_HPP

//---------------------------------------------------------------------------//
// end DTK_TransferOperator.hpp
//---------------------------------------------------------------------------//

