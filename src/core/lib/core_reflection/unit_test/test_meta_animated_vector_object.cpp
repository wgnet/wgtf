#include "pch.hpp"
#include "core_dependency_system/depends.hpp"
#include "test_meta_animated_vector_object.hpp"

#include "wg_types/vector4.hpp"

namespace wgt
{

Variant TestMetaAnimatedLerpVector4Obj::extrapolateValue(const Variant& lowerVariant,
	const Variant& higherVariant,
	float percent) /* override */
{
	Vector4 lowerValue;
	auto success = lowerVariant.tryCast<Vector4>(lowerValue);
	if (!success)
	{
		return Variant();
	}

	Vector4 higherValue;
	success = higherVariant.tryCast<Vector4>(higherValue);
	if (!success)
	{
		return Variant();
	}

	lowerValue.lerp(lowerValue, higherValue, percent);
	return Variant(lowerValue);
}

} // end namespace wgt
