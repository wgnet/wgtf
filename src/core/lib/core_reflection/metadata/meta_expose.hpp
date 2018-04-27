#pragma once

#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

namespace wgt
{
BEGIN_EXPOSE(MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaNoneObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaComponentObj, MetaBase, MetaNone())
EXPOSE("componentName", componentName_)
END_EXPOSE()

BEGIN_EXPOSE(MetaAngleObj, MetaComponentObj, MetaNone())
EXPOSE("convertToRadians", convertToRadians_)
END_EXPOSE()

BEGIN_EXPOSE(MetaTimeObj, MetaComponentObj, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaSignalObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaInvalidatesObjectObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaCallbackObj, MetaBase, MetaNone())
EXPOSE_METHOD("invoke", exposedInvoke)
END_EXPOSE()

BEGIN_EXPOSE(MetaMinMaxObj, MetaBase, MetaNone())
EXPOSE("min", getMin)
EXPOSE("max", getMax)
END_EXPOSE()

BEGIN_EXPOSE(MetaStepSizeObj, MetaBase, MetaNone())
EXPOSE("stepSize", getStepSize)
END_EXPOSE()

BEGIN_EXPOSE(MetaDecimalsObj, MetaBase, MetaNone())
EXPOSE("decimals", getDecimals)
END_EXPOSE()

BEGIN_EXPOSE(MetaEnumObj, MetaBase, MetaNone())
EXPOSE("enumString", getEnumString)
EXPOSE_METHOD("generateEnum", generateEnum)
END_EXPOSE()

BEGIN_EXPOSE(MetaSliderObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaDisplayNameObj, MetaBase, MetaNone())
EXPOSE("displayName", displayName_)
END_EXPOSE()

BEGIN_EXPOSE(MetaAttributeDisplayNameObj, MetaBase, MetaNone())
EXPOSE("attributeName", getAttributeName)
END_EXPOSE()

BEGIN_EXPOSE(MetaDisplayNameCallbackObj, MetaDisplayNameObj, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaDescriptionObj, MetaBase, MetaNone())
EXPOSE("description", getDescription)
END_EXPOSE()

BEGIN_EXPOSE(MetaPanelLayoutObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaNoNullObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaColorObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaHiddenObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaThumbnailObj, MetaBase, MetaNone())
EXPOSE("width", getWidth)
EXPOSE("height", getHeight)
END_EXPOSE()

BEGIN_EXPOSE(MetaInPlaceObj, MetaBase, MetaNone())
EXPOSE("propName", getPropName)
END_EXPOSE()

BEGIN_EXPOSE(MetaSelectedObj, MetaBase, MetaNone())
EXPOSE("propName", getPropName)
END_EXPOSE()

BEGIN_EXPOSE(MetaHDRColorReinhardTonemapObj, MetaHDRColorObj, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaGroupObj, MetaBase, MetaAttributeDisplayName("groupName"))
EXPOSE("groupName", groupName_)
END_EXPOSE()

BEGIN_EXPOSE(MetaGroupCallbackObj, MetaGroupObj, MetaAttributeDisplayName("groupName"))
END_EXPOSE()

BEGIN_EXPOSE(MetaUrlObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaPasswordObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaMultilineObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaDirectInvokeObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaInPlacePropertyNameObj, MetaBase, MetaNone())
EXPOSE("propertyName", getPropertyName)
END_EXPOSE()

BEGIN_EXPOSE(MetaReadOnlyObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaUniqueIdObj, MetaBase, MetaNone())
EXPOSE("id", getId)
END_EXPOSE()

BEGIN_EXPOSE(MetaCommandObj, MetaBase, MetaNone())
EXPOSE("commandName", getCommandName)
END_EXPOSE()

BEGIN_EXPOSE(MetaActionObj, MetaBase, MetaNone())
EXPOSE("actionName", getActionName)
EXPOSE_METHOD("execute", execute)
END_EXPOSE()

BEGIN_EXPOSE(MetaNoSerializationObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaOnStackObj, MetaBase, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaHDRColorObj, MetaBase, MetaNone())
EXPOSE_METHOD("tonemap", tonemap, MetaDirectInvoke())
EXPOSE("shouldUpdate", shouldUpdate, MetaSignalFunc(shouldUpdateSignal))
END_EXPOSE()

BEGIN_EXPOSE(MetaDisplayPathNameCallbackObj, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(MetaCollectionItemMetaObj, MetaBase, MetaNone())
END_EXPOSE()

} // end namespace wgt
