#pragma once

#include "core_common/assert.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"

namespace wgt	
{
struct GrabberUtils
{
	static std::string findTitle(IDefinitionManager& manager, const ObjectHandle& handle)
	{
		const IClassDefinition* definition = manager.getObjectDefinition(handle);
		TF_ASSERT(definition != nullptr);
		ObjectHandleT<MetaDisplayNameObj> displayNameObj = findFirstMetaData<MetaDisplayNameObj>(*definition, manager);
		return displayNameObj->getDisplayName() ?
			StringUtils::to_string(displayNameObj->getDisplayName()) :
			definition->getName();
	}

	static void findPropertyNames(IClassDefinition& definition, 
	                              IDefinitionManager& manager,
	                              std::vector<std::string>& names,
	                              std::function<bool(const char*)> acceptFn = nullptr)
	{
		auto propertyIteratorRange = definition.allProperties();
		auto itrFrom = propertyIteratorRange.begin();
		auto itrTo = propertyIteratorRange.end();

		for (auto itr = itrFrom; itr != itrTo; ++itr)
		{
			auto property = itr.get();
			auto metaAction = findFirstMetaData<MetaActionObj>(*property, manager);

			if (!metaAction.get() && (!acceptFn || acceptFn(property->getName())))
			{
				names.push_back(property->getName());
			}
		}
	}
};
} // end namespace wgt