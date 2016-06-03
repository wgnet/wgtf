#include "class_definition_model_new.hpp"

#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include <iterator>

namespace wgt
{
namespace
{
	class ClassDefinitionItem : public AbstractListItem
	{
	public:
		ClassDefinitionItem( const IClassDefinition * definition ) 
			: definition_( definition )
		{}

		Variant getData( int column, size_t roleId ) const override
		{
			if (roleId == ValueRole::roleId_)
			{
				return ObjectHandle(
					const_cast< IClassDefinition * >( definition_ ) );
			}
			else if (roleId == ValueTypeRole::roleId_)
			{
				return TypeId::getType< ObjectHandle >().getName();
			}
			return Variant();
		}	
		
		bool setData( int column, size_t roleId, const Variant & data ) override
		{
			return false;
		}

	private:
		const IClassDefinition * definition_;
	};
}


ClassDefinitionModelNew::ClassDefinitionModelNew( const IClassDefinition * definition,
	const IDefinitionManager & definitionManager )
{
	std::vector< IClassDefinition * > definitions;
	definitionManager.getDefinitionsOfType(
		definition, definitions );

	for (auto it = definitions.begin(); it != definitions.end(); ++it)
	{
		items_.emplace_back( new ClassDefinitionItem( *it ) );
	}
}


ClassDefinitionModelNew::~ClassDefinitionModelNew()
{
}


AbstractItem * ClassDefinitionModelNew::item( int row ) const /* override */
{
	assert( row >= 0 );
	const auto index = static_cast< std::vector< AbstractItem * >::size_type >( row );
	assert( index < items_.size() );
	return items_.at( index ).get();
}


int ClassDefinitionModelNew::index( const AbstractItem * item ) const /* override */
{
	auto it = std::find_if( items_.cbegin(), items_.cend(),
		[ item ]( const std::unique_ptr< AbstractItem > & next )
		{
			return next.get() == item;
		} );
	assert( it != items_.cend() );
	return static_cast< int >( std::distance( items_.cbegin(), it ) );
}


int ClassDefinitionModelNew::rowCount() const /* override */
{
	return static_cast< int >( items_.size() );
}


int ClassDefinitionModelNew::columnCount() const /* override */
{
	return 1;
}
} // end namespace wgt
