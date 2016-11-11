#ifndef I_CLASS_DEFINITION_MODIFIER_HPP
#define I_CLASS_DEFINITION_MODIFIER_HPP

#include <memory>

namespace wgt
{
class IBaseProperty;
class IDefinitionManager;
class MetaBase;

typedef std::shared_ptr<IBaseProperty> IBasePropertyPtr;

class IClassDefinitionModifier
{
public:
	virtual ~IClassDefinitionModifier()
	{
	}
	virtual IBasePropertyPtr addProperty(const char* name, const TypeId& typeId, MetaHandle metaData) = 0;
};
} // end namespace wgt
#endif // I_CLASS_DEFINITION_MODIFIER_HPP
