#include "signal_provider_impl.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
VariantSignal* SignalProviderImpl::getSignal(const ObjectHandle& provider, const IDefinitionManager& definitionManager)
{
	Variant value = baseProperty_->get(provider, definitionManager);
	VariantSignal* signal;
	bool ok = value.tryCast(signal);
	TF_ASSERT(ok);
	return signal;
}

} // end namespace wgt
