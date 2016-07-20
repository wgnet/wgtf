#include "qt_action_manager.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_ui_framework/i_action.hpp"

namespace wgt
{

class QtAction : public IAction
{
public:
	static const char pathDelimiter;

	QtAction( QtActionManager& actionManager, const char * text,
		const char * icon,
		const char * windowId, 
		const char * path,
		const char * shortcut,
		int order,
		std::function<void( IAction* )> & func, 
		std::function<bool( const IAction* )> & enableFunc,
		std::function<bool( const IAction* )> & checkedFunc )
		: actionManager_( actionManager )
		, text_( text )
		, icon_( icon )
		, windowId_( windowId )
		, paths_(StringUtils::split(path, pathDelimiter))
		, shortcut_( shortcut )
		, order_( order )
		, func_( func )
		, enableFunc_( enableFunc )
		, checkedFunc_( checkedFunc )
		, checkable_( checkedFunc ? true : false )
	{

	}

	~QtAction()
	{
		actionManager_.onQtActionDestroy( this );
	}

	const char * text() const override
	{
		return text_.c_str();
	}

	const char * icon() const override
	{
		return icon_.c_str();
	}

	const char * windowId() const override
	{
		return windowId_.c_str();
	}

	const std::vector<std::string>& paths() const override
	{
		return paths_;
	}

	const char * shortcut() const override
	{
		return shortcut_.c_str();
	}

	void setShortcut( const char * shortcut) override
	{
		shortcut_ = shortcut;
		signalShortcutChanged( shortcut );
	}

	int order() const override
	{
		return order_;
	}

	bool enabled() const override
	{
		return enableFunc_( this );
	}

	bool checked() const override
	{
		return checkedFunc_( this );
	}

	bool isCheckable() const override
	{
		return checkable_;
	}
	
	void execute() override
	{
		func_( this );
	}

	virtual void setData( const Variant& data ) override
	{
		data_ = data;
	}

	Variant& getData() override
	{
		return data_;
	}

	const Variant& getData() const override
	{
		return data_;
	}

private:
	QtActionManager& actionManager_;
	std::string text_;
	std::string icon_;
	std::string windowId_;
	std::vector<std::string> paths_;
	std::string shortcut_;
	int order_;
	std::function<void( IAction* )> func_;
	std::function<bool( const IAction* )> enableFunc_;
	std::function<bool( const IAction* )> checkedFunc_;
	Variant data_;
	bool checkable_;
};

const char QtAction::pathDelimiter = ';';


QtActionManager::QtActionManager( IComponentContext & contextManager )
	:base( contextManager )
{

}

QtActionManager::~QtActionManager()
{

}

std::unique_ptr<IAction> QtActionManager::createAction( const char* text, const char* icon, const char* windowId, 
													   const char* path, const char* shortcut, int order,
													   std::function<void( IAction*) > func, 
													   std::function<bool( const IAction* )> enableFunc, 
													   std::function<bool( const IAction* )> checkedFunc)
{
	return std::unique_ptr< IAction >( 
		new QtAction( *this, text, icon, windowId, path, shortcut, order, func, enableFunc, checkedFunc ) );
}

void QtActionManager::onQtActionDestroy( IAction* action )
{
	this->onActionDestroyed( action );
}

} // end namespace wgt
