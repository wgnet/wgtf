#ifndef CONTEXT_DEFINITION_MANAGER_HPP
#define CONTEXT_DEFINITION_MANAGER_HPP

#include <set>

#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class GenericProperty;

class ContextDefinitionManager : public Implements<IDefinitionManager>
{
public:
	ContextDefinitionManager(const wchar_t* contextName);
	virtual ~ContextDefinitionManager();

	void init(IDefinitionManager* pBaseManager, 
              std::function<IObjectManager*()> getContextObjManager, 
              std::function<void(void)> onContextDestroy);

	IDefinitionManager* getBaseManager() const;

private:
	// IDefinitionManager
	virtual IClassDefinition* getDefinition(const char* name) const override;
	virtual IClassDefinition* findDefinition(const char* name) const override;
	virtual IClassDefinition* getObjectDefinition(const ObjectHandle& object) const override;

	IClassDefinition* registerDefinition(std::unique_ptr<IClassDefinitionDetails> definition) override;
	bool deregisterDefinition(const IClassDefinition* definition) override;
    virtual void deregisterDefinitions() override;

	bool serializeDefinitions(ISerializer& serializer) override;
	bool deserializeDefinitions(ISerializer& serializer) override;

	void getDefinitionsOfType(const IClassDefinition* definition,
	                          std::vector<IClassDefinition*>& o_Definitions) const override;

	void getDefinitionsOfType(const std::string& type, std::vector<IClassDefinition*>& o_Definitions) const override;

	IObjectManager* getObjectManager() const override;

	void registerDefinitionHelper(const IDefinitionHelper& helper) override;
	void deregisterDefinitionHelper(const IDefinitionHelper& helper) override;

	void registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) override;

	void deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) override;

	const PropertyAccessorListeners& getPropertyAccessorListeners() const override;

	std::unique_ptr<IClassDefinitionDetails> createGenericDefinition(const char* name) const override;

private:
	mutable IObjectManager* contextObjManager_ = nullptr;
	std::function<IObjectManager*()> getContextObjManager_ = nullptr;
    std::function<void(void)> onContextDestroy_ = nullptr;
	IDefinitionManager* pBaseManager_ = nullptr;
	std::set<const IClassDefinition*> contextDefinitions_;
	const std::wstring contextName_;
};
} // end namespace wgt
#endif // CONTEXT_DEFINITION_MANAGER_HPP
