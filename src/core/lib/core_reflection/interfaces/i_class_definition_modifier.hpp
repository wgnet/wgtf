#ifndef I_CLASS_DEFINITION_MODIFIER_HPP
#define I_CLASS_DEFINITION_MODIFIER_HPP

#include <memory>
#include "core_common/signal.hpp"

namespace wgt
{
class IDefinitionManager;
class MetaData;
typedef std::shared_ptr<class IBaseProperty> IBasePropertyPtr;

class IClassDefinitionModifier
{
	typedef Signal<void(const char* name)> SignalDefinition;

public:
	virtual ~IClassDefinitionModifier()
	{
	}

	virtual IBasePropertyPtr addProperty(const char* name, const TypeId& typeId, MetaData metaData,
	                                     bool isCollection) = 0;

	virtual void removeProperty(const char* name) = 0;
	SignalDefinition prePropertyAdded;
	SignalDefinition postPropertyAdded;
	SignalDefinition prePropertyRemoved;
	SignalDefinition postPropertyRemoved;
};
} // end namespace wgt
#endif // I_CLASS_DEFINITION_MODIFIER_HPP
