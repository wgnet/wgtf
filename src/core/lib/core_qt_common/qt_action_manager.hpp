#ifndef QT_ACTION_MANAGER_HPP
#define QT_ACTION_MANAGER_HPP

#include <functional>
#include <map>
#include <memory>

class QIODevice;

namespace wgt
{
class IAction;
struct QtActionData;

class QtActionManager
{
public:
	QtActionManager();
	~QtActionManager();
	
	std::unique_ptr< IAction > createAction(
		const char * id,
		std::function<void( IAction* )> func,
		std::function<bool( const IAction* )> enableFunc,
		std::function<bool( const IAction* )> checkedFunc );

	void loadActionData( QIODevice & source );
	bool registerActionData( const char * id, 
		std::unique_ptr< QtActionData > & actionData );

private:
	std::map< std::string, std::unique_ptr< QtActionData > > actionData_;
};
} // end namespace wgt
#endif//ACTION_MANAGER_HPP
