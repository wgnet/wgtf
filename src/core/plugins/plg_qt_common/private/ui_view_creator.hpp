#ifndef UI_VIEW_CREATOR_HPP
#define UI_VIEW_CREATOR_HPP

#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"

#include <memory>

namespace wgt
{
class ObjectHandle;
class IComponentContext;
class IUIFramework;

class UIViewCreator
	: public Implements< IViewCreator >
	, public Depends< IUIFramework, IUIApplication >
{
public:
	UIViewCreator(IComponentContext & context);

	void createView(
		const char * path,
		const ObjectHandle & context,
		std::unique_ptr< IView > & o_ResultView,
		const char * uniqueName ) override;

	void createView(
		const char * path,
		const ObjectHandle & context,
		std::function< void(std::unique_ptr< IView > &) >,
		const char * uniqueName ) override;
};
} // end namespace wgt
#endif //UI_VIEW_CREATOR_HPP
