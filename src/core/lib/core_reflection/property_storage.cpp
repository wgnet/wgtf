#include "property_storage.hpp"
#include "property_iterator.hpp"

#include "interfaces/i_base_property.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{
class PropertyStorageIterator : public PropertyIteratorImplBase
{
public:
	PropertyStorageIterator(const PropertyStorage& storage);

	virtual std::shared_ptr<IBaseProperty> current() const override;

	virtual bool next() override;

private:
	IBasePropertyPtr current_;
	SortedProperties::const_iterator iterator_;
	SortedProperties::const_iterator end_;
};

PropertyStorage::PropertyStorage()
{
}

PropertyStorage::~PropertyStorage()
{
}


//------------------------------------------------------------------------------
void PropertyStorage::addProperty(const IBasePropertyPtr& property)
{
	properties_.emplace_back(property);
	propertyLookupMap_.insert(
		std::make_pair(property->getNameHash(), property));
}


//------------------------------------------------------------------------------
void PropertyStorage::removeProperty(const char* name)
{
	auto nameHash = HashUtilities::compute(name);
	auto found = std::find_if(properties_.begin(), properties_.end(), [nameHash](const IBasePropertyPtr& property) {
		return property->getNameHash() == nameHash;
	});
	if (found != properties_.end())
	{
		properties_.erase(found);
		propertyLookupMap_.erase(nameHash);
	}
}

//------------------------------------------------------------------------------
IBasePropertyPtr PropertyStorage::findProperty(const char * name) const
{
	auto nameHash = HashUtilities::compute(name);
	auto findIt = propertyLookupMap_.find(nameHash);
	if (findIt != propertyLookupMap_.end())
	{
		return findIt->second.lock();
	}
	return IBasePropertyPtr();
}


//------------------------------------------------------------------------------
IBasePropertyPtr PropertyStorage::findProperty( IPropertyPath::ConstPtr & path ) const
{
	auto findIt = propertyLookupMap_.find(path->getHash());
	if (findIt != propertyLookupMap_.end())
	{
		return findIt->second.lock();
	}
	return IBasePropertyPtr();
}

//------------------------------------------------------------------------------
PropertyIteratorImplPtr PropertyStorage::getIterator() const
{
	return PropertyIteratorImplPtr(new PropertyStorageIterator(*this));
}

PropertyStorageIterator::PropertyStorageIterator(const PropertyStorage& storage)
    : current_(nullptr), iterator_(storage.properties_.cbegin()), end_(storage.properties_.cend())
{
}

IBasePropertyPtr PropertyStorageIterator::current() const
{
	return current_;
}

bool PropertyStorageIterator::next()
{
	if (iterator_ == end_)
	{
		return false;
	}

	current_ = *iterator_;
	++iterator_;
	return true;
}
} // end namespace wgt
