#pragma once

#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/reflection_macros.hpp"

namespace wgt
{
struct ReflectionTypes
{
	static void init(IDefinitionManager& definitionManager)
	{
		REGISTER_DEFINITION(MetaNoneObj);
		REGISTER_DEFINITION(MetaAngleObj)
		REGISTER_DEFINITION(MetaTimeObj)
		REGISTER_DEFINITION(MetaBase);
		REGISTER_DEFINITION(MetaComponentObj)
		REGISTER_DEFINITION(MetaMinMaxObj);
		REGISTER_DEFINITION(MetaStepSizeObj);
		REGISTER_DEFINITION(MetaDecimalsObj);
		REGISTER_DEFINITION(MetaEnumObj);
		REGISTER_DEFINITION(MetaSliderObj);
		REGISTER_DEFINITION(MetaAttributeDisplayNameObj);
		REGISTER_DEFINITION(MetaGroupObj);
		REGISTER_DEFINITION(MetaGroupCallbackObj);
		REGISTER_DEFINITION(MetaDisplayNameObj);
		REGISTER_DEFINITION(MetaDisplayNameCallbackObj);
		REGISTER_DEFINITION(MetaDisplayPathNameCallbackObj);
		REGISTER_DEFINITION(MetaDescriptionObj);
		REGISTER_DEFINITION(MetaNoNullObj);
		REGISTER_DEFINITION(MetaColorObj);
		REGISTER_DEFINITION(MetaHDRColorObj);
		REGISTER_DEFINITION(MetaHDRColorReinhardTonemapObj);
		REGISTER_DEFINITION(MetaThumbnailObj);
		REGISTER_DEFINITION(MetaInPlaceObj);
		REGISTER_DEFINITION(MetaSelectedObj);
		REGISTER_DEFINITION(MetaHiddenObj);
		REGISTER_DEFINITION(MetaPanelLayoutObj);
		REGISTER_DEFINITION(MetaActionObj);
		REGISTER_DEFINITION(MetaCommandObj);
		REGISTER_DEFINITION(MetaNoSerializationObj);
		REGISTER_DEFINITION(MetaUniqueIdObj);
		REGISTER_DEFINITION(MetaOnStackObj);
		REGISTER_DEFINITION(MetaInPlacePropertyNameObj);
		REGISTER_DEFINITION(MetaReadOnlyObj);
		REGISTER_DEFINITION(MetaUrlObj);
		REGISTER_DEFINITION(MetaSignalObj);
		REGISTER_DEFINITION(MetaCollectionItemMetaObj);
		REGISTER_DEFINITION(MetaDirectInvokeObj);
		REGISTER_DEFINITION(MetaInvalidatesObjectObj);
		REGISTER_DEFINITION(MetaCallbackObj);
	}
};

namespace Reflection
{
	inline void initReflectedTypes(IDefinitionManager& definitionManager)
	{
		ReflectionTypes::init(definitionManager);
	}
}
} // end namespace wgt
