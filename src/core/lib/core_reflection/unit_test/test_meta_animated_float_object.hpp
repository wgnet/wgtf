#pragma once

#include "core_object/managed_object.hpp"
#include "core_reflection/metadata/meta_base.hpp"

#include "test_meta_animated_object.hpp"

namespace wgt
{

/**
 *	Test implementation of animated properties.
 *	Wraps sample float data.
 */
class TestMetaAnimatedLerpFloatObj : public TestMetaAnimatedObj
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
inline MetaData TestMetaAnimatedLerpFloat()
{
	return MetaData::create<TestMetaAnimatedLerpFloatObj>();
}

} // end namespace wgt

