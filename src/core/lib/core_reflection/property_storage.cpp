#include "property_storage.hpp"
#include "property_iterator.hpp"

#include "interfaces/i_base_property.hpp"

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

void PropertyStorage::addProperty(const IBasePropertyPtr& property)
{
	properties_.emplace_back(property);
}

PropertyIteratorImplPtr PropertyStorage::getIterator() const
{
	return std::make_shared<PropertyStorageIterator>(*this);
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
