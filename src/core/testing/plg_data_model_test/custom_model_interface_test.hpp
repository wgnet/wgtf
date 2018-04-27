#ifndef CUSTOM_MODEL_INTERFACE_TEST__HPP
#define CUSTOM_MODEL_INTERFACE_TEST__HPP

#include <memory>
#include "core_common/wg_future.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class TestFixture;
class IView;

class CustomModelInterfaceTest : public Depends<IViewCreator, IDefinitionManager, IFileSystem, IUIApplication>
{
public:
	~CustomModelInterfaceTest();

	void initialise();
	void fini();

private:
    ManagedObject<TestFixture> testFixture_;
	wg_future<std::unique_ptr<IView>> testView_;
};
} // end namespace wgt
#endif // CUSTOM_MODEL_INTERFACE_TEST__HPP
