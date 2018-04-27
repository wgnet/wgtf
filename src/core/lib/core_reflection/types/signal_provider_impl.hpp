#ifndef SIGNAL_PROVIDER_IMPL_HPP
#define SIGNAL_PROVIDER_IMPL_HPP

#include "core_reflection/interfaces/i_signal_provider.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"

namespace wgt
{
class REFLECTION_DLL SignalProviderImpl : public ISignalProvider
{
public:
	SignalProviderImpl(const IBasePropertyPtr& baseProperty) : baseProperty_(baseProperty)
	{
	}
	virtual ~SignalProviderImpl()
	{
	}
	VariantSignal* getSignal(const ObjectHandle& provider, const IDefinitionManager& definitionManager) override;

private:
	IBasePropertyPtr baseProperty_;
};
} // end namespace wgt
#endif // REFLECTED_COLLECTION_IMPL_HPP
