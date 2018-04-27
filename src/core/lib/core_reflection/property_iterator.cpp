#include "property_iterator.hpp"
#include "interfaces/i_class_definition.hpp"
#include "interfaces/i_class_definition_details.hpp"
#include "i_definition_manager.hpp"
#include "interfaces/i_base_property.hpp"

namespace wgt
{

struct PropertyIterator::ParentDefinitionIterator
{
	ParentDefinitionIterator(const IClassDefinition & definition)
		: visitedCurrent_(false)
		, current_(nullptr)
		, iterator_(definition.getParentNames().cbegin())
		, end_(definition.getParentNames().cend())
		, definitionManager_(*definition.getDefinitionManager())
	{
		next();
	}

	IClassDefinition * getCurrent() const
	{
		return current_;
	}

	bool next()
	{
		if (parentIterator_)
		{
			current_ = parentIterator_->getCurrent();
			if( parentIterator_->next())
			{
				return true;
			}
			if (current_ != nullptr)
			{
				parentIterator_ = nullptr;
				return true;
			}
		}

		if (iterator_ == end_)
		{
			current_ = nullptr;
			return false;
		}

		current_ = definitionManager_.getDefinition(iterator_->c_str());
		if (current_)
		{
			parentIterator_ = std::make_shared< ParentDefinitionIterator >(*current_);
		}
		++iterator_;
		return true;
	}

	typedef std::vector< std::string > ParentCollection;

	bool visitedCurrent_;
	IClassDefinition * current_;
	ParentCollection::const_iterator iterator_;
	ParentCollection::const_iterator end_;

	IDefinitionManager & definitionManager_;
	std::shared_ptr< ParentDefinitionIterator > parentIterator_;
};

// =============================================================================
PropertyIterator::PropertyIterator() : strategy_(ITERATE_SELF_ONLY), currentDefinition_(nullptr)
{
}

// =============================================================================
PropertyIterator::PropertyIterator(IterateStrategy strategy, const IClassDefinition& definition)
    : strategy_(strategy), currentDefinition_(&definition),
      currentIterator_(definition.getDetails().getPropertyIterator())
{
	moveNext();
}


//------------------------------------------------------------------------------
PropertyIterator::~PropertyIterator()
{
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
PropertyIterator& PropertyIterator::operator++()
{
	moveNext();
	return *this;
}

// =============================================================================
bool PropertyIterator::operator==(const PropertyIterator& other) const
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
bool PropertyIterator::operator!=(const PropertyIterator& other) const
{
	return !operator==(other);
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
	if (strategy_ == ITERATE_PARENTS)
	{
		if (parentIterator_ == nullptr)
		{
			parentIterator_ = std::make_shared< ParentDefinitionIterator >(*currentDefinition_);
			currentDefinition_ = parentIterator_->getCurrent();
			if (currentDefinition_ != nullptr)
			{
				currentIterator_ =
					currentDefinition_->getDetails().getPropertyIterator();
				if (currentIterator_->next())
				{
					return;
				}
				currentIterator_ = nullptr;
			}
		}

		currentDefinition_ = nullptr;
		while (parentIterator_->next())
		{
			currentDefinition_ = parentIterator_->getCurrent();
			if (currentDefinition_ == nullptr)
			{
				continue;
			}
			currentIterator_ =
				currentDefinition_->getDetails().getPropertyIterator();
			if (currentIterator_->next())
			{
				return;
			}
		}
	}
	else
	{
		currentDefinition_ = nullptr;
	}
	if (currentDefinition_ == nullptr)
	{
		return;
	}

	currentIterator_ = currentDefinition_->getDetails().getPropertyIterator();
	moveNext();
}

// =============================================================================
PropertyIteratorRange::PropertyIteratorRange(PropertyIterator::IterateStrategy strategy,
                                             const IClassDefinition& definition)
    : strategy_(strategy), definition_(definition)
{
}

// =============================================================================
PropertyIterator PropertyIteratorRange::begin() const
{
	return PropertyIterator(strategy_, definition_);
}

// =============================================================================
PropertyIterator PropertyIteratorRange::end() const
{
	return PropertyIterator();
}
} // end namespace wgt
