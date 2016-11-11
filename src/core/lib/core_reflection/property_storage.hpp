#ifndef PROPERTY_STORAGE_HPP
#define PROPERTY_STORAGE_HPP

#include <vector>
#include <memory>

#include "reflection_dll.hpp"

namespace wgt
{
class IBaseProperty;
typedef std::shared_ptr<IBaseProperty> IBasePropertyPtr;
typedef std::vector<IBasePropertyPtr> SortedProperties;
class PropertyIteratorImplBase;
typedef std::shared_ptr<PropertyIteratorImplBase> PropertyIteratorImplPtr;

class REFLECTION_DLL PropertyStorage
{
public:
	PropertyStorage();
	~PropertyStorage();

	void addProperty(const IBasePropertyPtr& property);
	PropertyIteratorImplPtr getIterator() const;

private:
	SortedProperties properties_;

	friend class PropertyStorageIterator;
};

} // end namespace wgt
#endif
