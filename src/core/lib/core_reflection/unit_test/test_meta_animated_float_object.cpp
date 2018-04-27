#include "pch.hpp"
#include "core_dependency_system/depends.hpp"
#include "test_meta_animated_float_object.hpp"

namespace wgt
{

float lerp(float x, float y, float s)
{
	return x + (s * (y - x));
}

Variant TestMetaAnimatedLerpFloatObj::extrapolateValue(const Variant& lowerVariant,
	const Variant& higherVariant,
	float percent) /* override */
{
	float lowerValue = 0.0f;
	auto success = lowerVariant.tryCast<float>(lowerValue);
	if (!success)
	{
		return Variant();
	}

	float higherValue = 0.0f;
	success = higherVariant.tryCast<float>(higherValue);
	if (!success)
	{
		return Variant();
	}

	return Variant(lerp(lowerValue, higherValue, percent));
}

} // end namespace wgt
