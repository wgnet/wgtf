#ifndef PROPERTY_STORAGE_HPP
#define PROPERTY_STORAGE_HPP

#include <vector>
#include <unordered_map>
#include <memory>
#include "core_reflection/interfaces/i_property_path.hpp"
#include "reflection_dll.hpp"

namespace wgt
{
class IBaseProperty;
typedef std::shared_ptr<IBaseProperty> IBasePropertyPtr;
typedef std::vector<IBasePropertyPtr> SortedProperties;
class PropertyIteratorImplBase;
typedef std::unique_ptr<PropertyIteratorImplBase> PropertyIteratorImplPtr;

class REFLECTION_DLL PropertyStorage
{
public:
	PropertyStorage();
	~PropertyStorage();

	void addProperty(const IBasePropertyPtr& property);
	void removeProperty(const char* name);
	IBasePropertyPtr findProperty(const char * name) const;
	IBasePropertyPtr findProperty(IPropertyPath::ConstPtr & path) const;
	PropertyIteratorImplPtr getIterator() const;

private:
	SortedProperties properties_;
	std::unordered_map< uint64_t, std::weak_ptr< IBaseProperty > > propertyLookupMap_;

	friend class PropertyStorageIterator;
};

} // end namespace wgt
#endif
