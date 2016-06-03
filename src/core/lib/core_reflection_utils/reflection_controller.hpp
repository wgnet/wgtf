#ifndef REFLECTION_CONTROLLER_HPP
#define REFLECTION_CONTROLLER_HPP

#include "core_reflection/interfaces/i_reflection_controller.hpp"

namespace wgt
{
class ICommandManager;

class ReflectionController
	: public Implements< IReflectionController >
{
public:
	ReflectionController();
	virtual ~ReflectionController();

	virtual void init( ICommandManager & commandManager );
	virtual void fini( );

	Variant getValue( const PropertyAccessor & pa ) override;
	void setValue( const PropertyAccessor & pa, const Variant & data ) override;
	Variant invoke( const PropertyAccessor & pa, const ReflectedMethodParameters & parameters ) override;
	void insert( const PropertyAccessor & pa, const Variant & key, const Variant & value ) override;
	void erase( const PropertyAccessor & pa, const Variant & key ) override;

private:
	class Impl;
	std::unique_ptr< Impl > impl_;
};
} // end namespace wgt
#endif //COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP
