#ifndef OBJECT_MANAGER_CREATOR_HPP
#define OBJECT_MANAGER_CREATOR_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include <memory>

namespace wgt
{
class IComponentContext;

class ObjectManagerCreator : public Implements< IComponentContextCreator >
{
public:
	ObjectManagerCreator( IComponentContext & context );
	virtual ~ObjectManagerCreator();

	const char * getType() const override;
	IInterface * createContext( const wchar_t * contextId ) override;

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif //OBJECT_MANAGER_CREATOR_HPP
