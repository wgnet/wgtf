#ifndef I_STATIC_INITIALIZER_HPP
#define I_STATIC_INITIALIZER_HPP

#include <memory>
#include <functional>

namespace wgt
{
class IComponentContext;

class IStaticInitalizer
{
public:
	typedef std::function<void(IComponentContext&)> ExecFunc;
	typedef std::function<void()> FiniFunc;

	virtual ~IStaticInitalizer()
	{
	}

	virtual void registerStaticInitializer(
		const std::shared_ptr< ExecFunc >&,
		const std::shared_ptr< FiniFunc > & ) = 0;

	virtual void initStatics(IComponentContext& context) = 0;

	virtual void destroy() = 0;
};
}

#endif // I_STATIC_INITIALIZER_HPP