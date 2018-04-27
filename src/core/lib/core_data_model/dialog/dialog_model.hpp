#pragma once

#include "core_ui_framework/i_dialog.hpp"
#include "core_common/assert.hpp"
#include "core_common/signal.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include <functional>

namespace wgt
{
class DialogModel
{
DECLARE_REFLECTED

public:
	DialogModel();
	virtual ~DialogModel();

	template <typename Model, typename... Args>
	static typename std::enable_if<std::is_base_of<DialogModel, Model>::value, ManagedObjectPtr>::type
	create(Args&&... args)
	{
		registerDefinition<Model>();
		return ManagedObject<Model>::make_unique(std::forward<Args>(args)...);
	}

	IDialog::Result result() const;
	virtual bool canClose() const;
	virtual IDialog::Result closeResult() const;
	virtual void onClose(IDialog::Result result);
	virtual void onShow();
	virtual const char* getUrl() const;
	virtual const char* getTitle() const;
	void setURL(const char* url);
	void setTitle(const char* title);

protected:
	void setResult(IDialog::Result result);

	template <typename Model>
	static void registerDefinition()
	{
		DependsLocal<IDefinitionManager> depends;
		auto definitionManager = depends.get<IDefinitionManager>();
		TF_ASSERT(definitionManager);
		if (!definitionManager->getDefinition<Model>())
		{
			definitionManager->registerDefinition<TypeClassDefinition<Model>>();
		}
	}

private:
	std::string url_;
	std::string title_;
	IDialog::Result result_;
	bool useStoredTitle_;
};
} // end namespace wgt
