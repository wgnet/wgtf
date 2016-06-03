#ifndef SHARED_CONTROLS_HPP
#define SHARED_CONTROLS_HPP

namespace wgt
{
class IDefinitionManager;
class ICommandManager;

class SharedControls
{
public:
	static void init();
	static void initDefs(
		IDefinitionManager & definitionManager );
};
} // end namespace wgt
#endif //SHARED_CONTROLS_HPP
