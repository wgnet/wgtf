#ifndef I_DEFINITION_MANAGER_HPP
#define I_DEFINITION_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>

#include "reflected_object.hpp"
#include "reflection_dll.hpp"
#include "interfaces/i_class_definition.hpp"

namespace wgt
{
class IClassDefinitionModifier;
class IClassDefinitionDetails;
class IDefinitionHelper;
class IObjectManager;
class PropertyAccessorListener;
class GenericObjectListener;
class ISerializer;
class GenericObjectListener;

/**
 * IDefinitionManager
 */
class REFLECTION_DLL IDefinitionManager
{
public:
	virtual ~IDefinitionManager()
	{
	}

	typedef std::vector<std::shared_ptr<PropertyAccessorListener>> PropertyAccessorListeners;


	/**
	 *	Get a definition for the type represented by 'name'.
	 */
	virtual IClassDefinition* getDefinition(const char* name) const = 0;

	/**
	* Finds the definition that includes 'name'
	*/
	virtual IClassDefinition* findDefinition(const char* name) const = 0;

	/**
	 *	Get the class definition for the object instance.
	 *	If it is a generic object, different instances may have different definitions.
	 *	e.g. if a property has been added at runtime to an object, it will
	 *	have a different definition to before.
	 */
	virtual IClassDefinition* getObjectDefinition(const ObjectHandle& object) const = 0;

	/**
	 *	Get the class definition for the object instance.
	 *	If it is a generic object, different instances may have different definitions.
	 *	e.g. if a property has been added at runtime to an object, it will
	 *	have a different definition to before.
	 */
	IClassDefinition* getDefinition(const ObjectHandle& object) const
	{
		return getObjectDefinition(object);
	}

	virtual std::unique_ptr<IClassDefinitionDetails> createGenericDefinition(const char* name) const = 0;

	virtual void getDefinitionsOfType(const IClassDefinition* definition,
	                                  std::vector<IClassDefinition*>& o_Definitions) const = 0;

	virtual void getDefinitionsOfType(const std::string& type, std::vector<IClassDefinition*>& o_Definitions) const = 0;

	virtual IObjectManager* getObjectManager() const = 0;

	virtual IClassDefinition* registerDefinition(std::unique_ptr<IClassDefinitionDetails> definition) = 0;
	virtual bool deregisterDefinition(const IClassDefinition* definition) = 0;
    virtual void deregisterDefinitions() = 0;

	virtual void registerDefinitionHelper(const IDefinitionHelper& helper) = 0;
	virtual void deregisterDefinitionHelper(const IDefinitionHelper& helper) = 0;

	virtual void registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) = 0;
	virtual void deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) = 0;
	virtual const PropertyAccessorListeners& getPropertyAccessorListeners() const = 0;

	virtual bool serializeDefinitions(ISerializer& serializer) = 0;
	virtual bool deserializeDefinitions(ISerializer& serializer) = 0;

	template <typename TargetType>
	IClassDefinition* getDefinition() const
	{
		const char* defName = getClassIdentifier<TargetType>();
		return getDefinition(defName);
	}

	template<class T>
	ObjectHandleStoragePtr createObjectStorage() const
	{
		if (definition = getDefinition<T>())
		{
			return definition->createObjectStorage();
		}

		return nullptr;
	}

	template <class TDefinition>
	IClassDefinition * registerDefinition()
	{
		return registerDefinition(std::unique_ptr<IClassDefinitionDetails>(new TDefinition()));
	}
};
} // end namespace wgt
#endif // I_DEFINITION_MANAGER_HPP
