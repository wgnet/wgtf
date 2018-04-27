#ifndef I_SIGNAL_PROVIDER_HPP
#define I_SIGNAL_PROVIDER_HPP

#include <memory>
#include "../reflection_dll.hpp"
#include "core_common/signal.hpp"

namespace wgt
{
class IDefinitionManager;
class ObjectHandle;
class Collection;
class Variant;

typedef Signal<void(Variant&)> VariantSignal;

class REFLECTION_DLL ISignalProvider
{
public:
	virtual ~ISignalProvider()
	{
	}
	virtual VariantSignal* getSignal(const ObjectHandle& provider, const IDefinitionManager& definitionManager) = 0;
};

typedef std::unique_ptr<ISignalProvider> ISignalProviderPtr;
} // end namespace wgt
#endif // I_ENUM_GENERATOR_HPP
