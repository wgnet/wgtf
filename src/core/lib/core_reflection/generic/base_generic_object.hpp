#ifndef BASE_GENERIC_OBJECT_HPP
#define BASE_GENERIC_OBJECT_HPP

#include "core_common/assert.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class ReflectedMethodParameters;
class Variant;
class PropertyAccessor;

/**
 *	GenericObject is an object that has a "generic type".
 *
 *	Generic types are not based on a static class definition, like a C++ class.
 *	Generic types are more like script classes which may share the same
 *	definition or be per instance because they can dynamically
 *	add and remove members.
 */
class REFLECTION_DLL BaseGenericObject
{
public:
	BaseGenericObject();
	virtual ~BaseGenericObject();

	/**
	 *	Get the definition associated with this generic object
	 *  @return definition object definition
	 */
	IClassDefinition* getDefinition() const;

	/**
	 *	Set the definition for this generic object
	 *  @param definition object definition
	 */
	void setDefinition(IClassDefinition* definition);

	/**
	 *	Get a typed property from the Python object.
	 *	@param name name of property.
	 *	@param outValue value of property is stored here.
	 *	@return true on success.
	 */
	template <typename T>
	bool get(const char* name, T& value) const
	{
		auto pDefinitionManager = this->getDefinition()->getDefinitionManager();
		TF_ASSERT(pDefinitionManager != nullptr);
		auto variant = this->getProperty(name);
		return ReflectionUtilities::extract(variant, value, (*pDefinitionManager));
	}

	/**
	 *	Set a typed property on the object.
	 *	@param name name of property.
	 *	@param value value of property to set.
	 *	@return true on success.
	 */
	template <typename T>
	bool set(const char* name, const T& value, bool enableNotification = true)
	{
		return this->setProperty(name, value, enableNotification);
	}

	/**
	 *	Set a variant property on the object.
	 *	@param name name of property.
	 *	@param value value of property to set.
	 *	@return true on success.
	 */
	bool set(const char* name, const Variant& value, bool enableNotification = true);

	/**
	 *	Adds a new variant property on the object.
	 *	@param name name of property.
	 *	@param value value of property to set.
	 *  @param metadata Metadata to attach to the property
	 *	@return true on success.
	 */
	bool add(const char* name, const Variant& value, MetaData metadata, bool enableNotification = true);

	/**
	 *	Call a function which is part of this instance.
	 *
	 *	@pre the given name must be a callable function on this instance.
	 *	@pre the given parameters must be passable to the function.
	 *
	 *	@param name the name of the function.
	 *	@param parameters the arguments to the function.
	 *	@return the return value of the function.
	 */
	Variant invoke(const char* name, const ReflectedMethodParameters& parameters);

	/**
	 *	Default implementation can be overridden by derived classes.
	 */
	virtual PropertyAccessor findProperty(const char* name) const;

protected:
	Variant invokeProperty(const char* name, const ReflectedMethodParameters& parameters);
	Variant getProperty(const char* name) const;
	bool setProperty(const char* name, const Variant& value, bool enableNotification);
	bool addProperty(const char* name, const Variant& value, MetaData metadata, bool enableNotification);

	/**
	 *	Must be implemented in all types that derive from BaseGenericObject.
	 *
	 *	ObjectHandle must be constructed from the most derived type.
	 *	If and ObjectHandleT< BaseGenericObject > is constructed, then
	 *	it will slice the class.
	 *
	 *	@return an ObjectHandle containing a pointer to the most derived type
	 *		from BaseGenericObject.
	 */
	virtual ObjectHandle getDerivedType() const = 0;
	virtual ObjectHandle getDerivedType() = 0;

	IClassDefinition* definition_;
};

} // end namespace wgt
#endif // BASE_GENERIC_OBJECT_HPP
