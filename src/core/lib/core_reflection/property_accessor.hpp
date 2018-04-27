#ifndef PROPERTY_ACCESSOR_HPP
#define PROPERTY_ACCESSOR_HPP

#include <string>
#include <memory>
#include "core_variant/collection.hpp"
#include "reflection_dll.hpp"
#include "core_reflection/metadata/meta_base.hpp"

namespace wgt
{
class TypeId;
class IClassDefinition;
class IDefinitionManager;
class ReflectedMethodParameters;
class ObjectReference;
class ObjectHandle;
typedef std::shared_ptr<class IBaseProperty> IBasePropertyPtr;

namespace PropertyAccessorPrivate
{
	struct Data;
}

class REFLECTION_DLL PropertyAccessor
{
public:
	PropertyAccessor();
	~PropertyAccessor();
	PropertyAccessor(const PropertyAccessor& other);
	PropertyAccessor(PropertyAccessor&& other);
	PropertyAccessor& operator=(const PropertyAccessor& other);
	PropertyAccessor& operator=(PropertyAccessor&& other);

	bool isValid() const;
	const TypeId& getType() const;
	const MetaData & getMetaData() const;
	const char* getName() const;
	const IClassDefinition* getStructDefinition() const;
	const ObjectHandle & getRootObject() const;
	const char* getFullPath() const;
	const IDefinitionManager* getDefinitionManager() const;

	// TODO: hide these accessors
	bool canGetValue() const;
	Variant getValue() const;

	bool canSetValue() const;
	void notify() const;
	bool setValue(const Variant& value) const;
	bool setValueWithoutNotification(const Variant& value) const;

	bool canInvoke() const;
	Variant invoke(const ReflectedMethodParameters& parameters) const;
	void invokeUndoRedo(const ReflectedMethodParameters& parameters, Variant result, bool undo) const;

	bool canInsert() const;
	bool insert(const Variant& key, const Variant& value) const;

	bool canErase() const;
	bool erase(const Variant& key) const;

	IBasePropertyPtr getProperty() const;
	const ObjectHandle& getObject() const;
	const char * getPath() const;

private:
	friend class ClassDefinition;

	std::shared_ptr< PropertyAccessorPrivate::Data > data_;

	PropertyAccessor(const char* path, const std::shared_ptr<ObjectReference>& reference);
	PropertyAccessor( std::shared_ptr< PropertyAccessorPrivate::Data > &);
	std::shared_ptr< PropertyAccessorPrivate::Data > & getData ();
	void setObjectReference(const std::shared_ptr<ObjectReference>& reference);
	void setPath(const std::string& path);
	void setBaseProperty(const IBasePropertyPtr& property);
	void propagateSetToAncestors() const;
};
} // end namespace wgt
#endif // PROPERTY_ACCESSOR_HPP
