#include "object_selection_helper.hpp"
#include "core_variant/variant.hpp"
#include "core_command_system/i_command_manager.hpp"


namespace wgt
{
ObjectSelectionHelper::ObjectSelectionHelper()
{

}

ObjectSelectionHelper::~ObjectSelectionHelper()
{

}


void ObjectSelectionHelper::init( ISelectionContext* selectionContext, const ObjectHandle & value )
{
	assert( selectionContext );
	selectionContext_ = selectionContext;
	value_ = value;
	selectionContext_->setContextObject( value_ );
}

Variant ObjectSelectionHelper::variantValue() const
{
	return Variant( value_ );
}


bool ObjectSelectionHelper::variantValue( const Variant& data )
{
	ObjectHandle typedValue;
	const bool ok = data.tryCast( typedValue );
	if (!ok)
	{
		return false;
	}

	this->value( typedValue );
	return true;
}

const ObjectHandle& ObjectSelectionHelper::value() const
{
	return value_;
}


void ObjectSelectionHelper::value( const ObjectHandle& data )
{
	if (value_ == data)
	{
		return;
	}
	assert( selectionContext_ );
	this->signalPreDataChanged();
	value_ = data;
	this->signalPostDataChanged();
	selectionContext_->setContextObject( value_ );
}
} // end namespace wgt
