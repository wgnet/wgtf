#ifndef PLUGIN_STATIC_INITIALIZER_HPP
#define PLUGIN_STATIC_INITIALIZER_HPP

#include <memory>
#include "core_dependency_system/i_interface.hpp"
#include "core/interfaces/common_include/i_static_initializer.hpp"

namespace wgt
{

//==============================================================================
class PluginStaticInitializer : public Implements<IStaticInitalizer>
{
	struct InitNode;

public:
	PluginStaticInitializer();
	virtual ~PluginStaticInitializer();
	void registerStaticInitializer(
		const std::shared_ptr< ExecFunc > & execFunc,
		const std::shared_ptr< FiniFunc > & finiFunc) override;
	void initStatics(IComponentContext& context) override;
	void destroy() override;

private:
	std::unique_ptr<InitNode> headInitNode_;
	InitNode* tailInitNode_;
};

} // end namespace wgt
#endif // PLUGIN_STATIC_INITIALIZER_HPP
