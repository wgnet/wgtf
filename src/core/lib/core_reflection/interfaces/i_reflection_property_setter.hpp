#ifndef I_REFLECTION_PROPERTY_SETTER
#define I_REFLECTION_PROPERTY_SETTER

#include "../generic/generic_object.hpp"

namespace wgt
{
class PropertyAccessor;

class IReflectionPropertySetter
{
public:
	virtual void setDataValue(const PropertyAccessor& pa, const Variant& data) = 0;
};
} // end namespace wgt
#endif // I_REFLECTION_PRO	PERTY_SETTER
