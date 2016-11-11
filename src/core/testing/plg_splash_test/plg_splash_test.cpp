#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_serialization/i_resource_system.hpp"
#include "interfaces/core_splash/i_splash.hpp"
#include "wg_types/binary_block.hpp"

#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
 *	A plugin which registers a custom splash screen.
 *
 * @ingroup plugins
 */
class SplashTestPlugin : public PluginMain
{
public:
	SplashTestPlugin(IComponentContext& contextManager);
	bool PostLoad(IComponentContext& contextManager) override;
};

class CustomSplash : public Implements<ISplash>, Depends<IResourceSystem>
{
public:
	CustomSplash(IComponentContext& context);
	bool loadData(std::unique_ptr<BinaryBlock>& o_data, std::string& o_format) const override;
};

SplashTestPlugin::SplashTestPlugin(IComponentContext& contextManager)
{
}

bool SplashTestPlugin::PostLoad(IComponentContext& contextManager) /* override */
{
	contextManager.registerInterface(new CustomSplash(contextManager));
	return true;
}

CustomSplash::CustomSplash(IComponentContext& context) : Depends<IResourceSystem>(context)
{
}

bool CustomSplash::loadData(std::unique_ptr<BinaryBlock>& o_data, std::string& o_format) const /* override */
{
	const auto resourceSystem = this->get<IResourceSystem>();
	if (resourceSystem == nullptr)
	{
		NGT_ERROR_MSG("Cannot load splash screen resource files.\n");
		return false;
	}
	o_data = resourceSystem->readBinaryContent(":/Splash/icons/splash.png");
	o_format = "PNG";
	return ((o_data != nullptr) && (o_data->length() > 0));
}

PLG_CALLBACK_FUNC(SplashTestPlugin)

} // end namespace wgt
