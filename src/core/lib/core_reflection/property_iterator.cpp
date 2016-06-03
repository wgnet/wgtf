#include "property_iterator.hpp"
#include "interfaces/i_class_definition.hpp"
#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_base_property.hpp"

namespace wgt
{
// =============================================================================
PropertyIterator::PropertyIterator()
	: strategy_(ITERATE_SELF_ONLY)
	, currentDefinition_( nullptr )
{
}

// =============================================================================
PropertyIterator::PropertyIterator( IterateStrategy strategy, const IClassDefinition & definition  )
	: strategy_( strategy )
	, currentDefinition_( &definition )
	, currentIterator_( definition.getDetails().getPropertyIterator() )
{
	moveNext();
}

// =============================================================================
IBasePropertyPtr PropertyIterator::get() const
{
	return currentIterator_ != nullptr ? currentIterator_->current() : nullptr;
}

// =============================================================================
IBasePropertyPtr PropertyIterator::operator*() const
{
	return get();
}

// =============================================================================
IBasePropertyPtr PropertyIterator::operator->() const
{
	return get();
}

// =============================================================================
PropertyIterator & PropertyIterator::operator++()
{
	moveNext();
	return *this;
}

// =============================================================================
bool PropertyIterator::operator==( const PropertyIterator& other ) const
{
	if (currentDefinition_ != other.currentDefinition_)
	{
		return false;
	}

	auto property = get();
	auto otherProperty = other.get();

	if (property == otherProperty)
	{
		return true;
	}

	if (property == nullptr || otherProperty == nullptr)
	{
		return false;
	}

	return property->getNameHash() == otherProperty->getNameHash();
}

// =============================================================================
bool PropertyIterator::operator!=( const PropertyIterator& other ) const
{
	return !operator==( other );
}

// =============================================================================
void PropertyIterator::moveNext()
{
	if (currentIterator_ == nullptr)
	{
		currentDefinition_ = nullptr;
		return;
	}

	if (currentIterator_->next())
	{
		return;
	}

	currentIterator_.reset();
	currentDefinition_ = strategy_ == ITERATE_PARENTS ? currentDefinition_->getParent() : nullptr;
	if (currentDefinition_ == nullptr)
	{
		return;
	}

	currentIterator_ = currentDefinition_->getDetails().getPropertyIterator();
	moveNext();
}

// =============================================================================
PropertyIteratorRange::PropertyIteratorRange( PropertyIterator::IterateStrategy strategy, const IClassDefinition & definition )
	: strategy_( strategy )
	, definition_( definition )
{
}

// =============================================================================
PropertyIterator PropertyIteratorRange::begin() const
{
	return PropertyIterator( strategy_, definition_ );
}

// =============================================================================
PropertyIterator PropertyIteratorRange::end() const
{
	return PropertyIterator();
}
} // end namespace wgt
