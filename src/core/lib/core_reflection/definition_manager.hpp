#ifndef DEFINITION_MANAGER_HPP
#define DEFINITION_MANAGER_HPP

#include <map>
#include <unordered_map>

#include "i_definition_manager.hpp"
#include "core_common/wg_read_write_lock.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "wg_types/hashed_string_ref.hpp"
#include "reflection_dll.hpp"

namespace wgt
{
/**
 * DefinitionManager
 */
class REFLECTION_DLL DefinitionManager : public Implements<IDefinitionManager>
{
public:
	// IDefinitionManager
	virtual IClassDefinition* getDefinition(const char* name) const override;
	virtual IClassDefinition* findDefinition(const char* name) const override;
	virtual IClassDefinition* getObjectDefinition(const ObjectHandle& object) const override;

	std::unique_ptr<IClassDefinitionDetails> createGenericDefinition(const char* name) const override;

	virtual void getDefinitionsOfType(const IClassDefinition* definition,
	                                  std::vector<IClassDefinition*>& o_Definitions) const override;

	virtual void getDefinitionsOfType(const std::string& type,
	                                  std::vector<IClassDefinition*>& o_Definitions) const override;

	void registerDefinitionHelper(const IDefinitionHelper& helper) override;
	void deregisterDefinitionHelper(const IDefinitionHelper& helper) override;

	void registerPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) override;
	void deregisterPropertyAccessorListener(std::shared_ptr<PropertyAccessorListener>& listener) override;
	const PropertyAccessorListeners& getPropertyAccessorListeners() const override;

	virtual IObjectManager* getObjectManager() const override;

	virtual IClassDefinition* registerDefinition(std::unique_ptr<IClassDefinitionDetails> definition) override;
	virtual bool deregisterDefinition(const IClassDefinition* definition) override;
    virtual void deregisterDefinitions() override;

	bool serializeDefinitions(ISerializer& serializer) override;
	bool deserializeDefinitions(ISerializer& serializer) override;

	DefinitionManager(IObjectManager& objectManager);
	virtual ~DefinitionManager();

private:
	mutable wg_read_write_lock lock_;
	typedef std::unordered_map<HashedStringRef, IClassDefinition*> ClassDefCollection;
	ClassDefCollection definitions_;

	void getDefinitionsOfType(IClassDefinition* definition, std::vector<IClassDefinition*>& o_Definitions,
	                          size_t startIndex) const;

	mutable wg_read_write_lock helpersLock_;
	std::map<TypeId, const IDefinitionHelper*> helpers_;
	std::unique_ptr<IDefinitionHelper> genericDefinitionHelper_;

	mutable wg_read_write_lock listenersLock_;
	PropertyAccessorListeners listeners_;
	IObjectManager& objectManager_;
};

} // end namespace wgt
#endif // DEFINITION_MANAGER_HPP
