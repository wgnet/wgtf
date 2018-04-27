#include "meta_callback_property_accessor_listener.hpp"

#include "metadata/meta_impl.hpp"
#include "metadata/meta_base.hpp"
#include "property_accessor.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
void MetaCallbackPropertyAccessorListener::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	const IDefinitionManager* definitionManager = accessor.getDefinitionManager();
	TF_ASSERT(definitionManager);

	auto metaCallback = findFirstMetaData<MetaCallbackObj>(accessor, *definitionManager);
	if (metaCallback != nullptr)
	{
		metaCallback->invoke(accessor.getObject());
	}
}
} // end namespace wgt
