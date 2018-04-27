#pragma once

#include "core_object/managed_object.hpp"
#include "core_reflection/metadata/meta_base.hpp"

#include "test_meta_animated_object.hpp"

namespace wgt
{

/**
 *	Test implementation of animated properties.
 *	Wraps sample vector4 data.
 */
class TestMetaAnimatedLerpVector4Obj : public TestMetaAnimatedObj
{
private:
	virtual Variant extrapolateValue(const Variant& lowerVariant,
		const Variant& higherVariant,
		float percent) override;
};

/**
 * @note cannot use IMetaTypeCreator because the test meta object cannot be
 *	added to the interface.
 */
inline MetaData TestMetaAnimatedLerpVector4()
{
	return MetaData::create<TestMetaAnimatedLerpVector4Obj>();
}

} // end namespace wgt

