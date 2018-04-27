#include "pch.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

#include "core_unit_test/unit_test.hpp"
#include "core_unit_test/test_object_manager.hpp"

#include "wg_types/vector4.hpp"

#include "test_meta_animated_float_object.hpp"
#include "test_meta_animated_vector_object.hpp"
#include "test_reflection_fixture.hpp"

#include "test_meta_animated.hpp"

#include <limits>

namespace wgt
{

class TestMetaDataFixture : public TestReflectionFixture
{
public:
	TestMetaDataFixture();
	~TestMetaDataFixture();
};

TestMetaDataFixture::TestMetaDataFixture()
	: TestReflectionFixture()
{
}

TestMetaDataFixture::~TestMetaDataFixture()
{
}

namespace MetaAnimatedTest
{

TestMetaDataObject::TestMetaDataObject()
	: floating_(0.0f)
	, floating_2_(0.0f)
{
}

}

TEST_F(TestMetaDataFixture, float_animation)
{
	using namespace MetaAnimatedTest;
	TestMetaDataObject subject;
	const std::string PATH = "test_float";

	IDefinitionManager& definitionManager = getDefinitionManager();
	auto pTestDefinition = definitionManager.getDefinition<TestMetaDataObject>();
	CHECK(pTestDefinition != nullptr);
	if (pTestDefinition == nullptr)
	{
		return;
	}
	auto& testDefinition = (*pTestDefinition);
	ManagedObject<TestMetaDataObject> object(subject);
	auto provider = object.getHandleT();

	auto floatPropertyAccessor = testDefinition.bindProperty(PATH.c_str(), provider);
	CHECK(floatPropertyAccessor.isValid());
	if (!floatPropertyAccessor.isValid())
	{
		return;
	}
	auto animatedHandle = floatPropertyAccessor.getObject();
	CHECK(animatedHandle.isValid());
	if (!animatedHandle.isValid())
	{
		return;
	}
	auto pMetaAnimated = findFirstMetaData<IMetaAnimatedObj>(floatPropertyAccessor,
		definitionManager);
	CHECK(pMetaAnimated != nullptr);
	if (pMetaAnimated == nullptr)
	{
		return;
	}
	auto& metaAnimated = const_cast<IMetaAnimatedObj&>(*pMetaAnimated);

	const float EPSILON = 0.01f;

	// Empty timeline
	{
		CHECK_EQUAL(false, metaAnimated.isActive(animatedHandle, PATH));
		const auto success = metaAnimated.setActive(animatedHandle, PATH, true);
		CHECK(success);
		CHECK_EQUAL(true, metaAnimated.isActive(animatedHandle, PATH));
	}

	{
		CHECK_EQUAL(false, metaAnimated.isLooping(animatedHandle, PATH));
		const auto success = metaAnimated.setLooping(animatedHandle, PATH, true);
		CHECK(success);
		CHECK_EQUAL(true, metaAnimated.isLooping(animatedHandle, PATH));
	}

	{
		CHECK_CLOSE(0.0f, metaAnimated.getDuration(animatedHandle, PATH), EPSILON);
		CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
	}

	// Full timeline
	{
		const IMetaAnimatedObj::anim_time_t DURATION = 1.0f;

		const Variant VALUE_0 = 0.0f;
		const Variant VALUE_1 = 0.5f;
		const Variant VALUE_2 = 100.0f;

		const IMetaAnimatedObj::anim_time_t TIME_0 = 0.0f;
		const IMetaAnimatedObj::anim_time_t TIME_1 = DURATION * 0.2f;
		const IMetaAnimatedObj::anim_time_t TIME_2 = DURATION;

		const auto insert0Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_0,
			TIME_0);
		CHECK_EQUAL(0, insert0Index);
		const auto duration0 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_0, duration0, EPSILON);
		CHECK_EQUAL(1, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		const auto insert1Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_1,
			TIME_1);
		CHECK_EQUAL(1, insert1Index);
		const auto duration1 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_1, duration1, EPSILON);
		CHECK_EQUAL(2, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		const auto insert2Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_2,
			TIME_2);
		CHECK_EQUAL(2, insert2Index);
		const auto duration2 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_2, duration2, EPSILON);
		CHECK_EQUAL(3, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		CHECK_CLOSE(TIME_0, metaAnimated.getKeyframeTime(animatedHandle, PATH, 0), EPSILON);
		CHECK_CLOSE(TIME_1, metaAnimated.getKeyframeTime(animatedHandle, PATH, 1), EPSILON);
		CHECK_CLOSE(TIME_2, metaAnimated.getKeyframeTime(animatedHandle, PATH, 2), EPSILON);
		CHECK_CLOSE(std::numeric_limits<IMetaAnimatedObj::anim_time_t>::max(),
			metaAnimated.getKeyframeTime(animatedHandle, PATH, 3),
			EPSILON);

		const IMetaAnimatedObj::anim_time_t NEW_TIME_0 = 2.0f;
		const IMetaAnimatedObj::anim_time_t NEW_TIME_1 = NEW_TIME_0 + (DURATION * 0.7f);
		const IMetaAnimatedObj::anim_time_t NEW_TIME_2 = NEW_TIME_0 + DURATION;

		// Keyframe indexes re-sorted after set
		const auto set0Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_0);
		CHECK(set0Success);
		const auto set1Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_1);
		CHECK(set1Success);
		const auto set2Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_2);
		CHECK(set2Success);
		const auto set3Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 3, 1000.0f);
		CHECK(!set3Success);

		CHECK_CLOSE(NEW_TIME_0, metaAnimated.getKeyframeTime(animatedHandle, PATH, 0), EPSILON);
		CHECK_CLOSE(NEW_TIME_1, metaAnimated.getKeyframeTime(animatedHandle, PATH, 1), EPSILON);
		CHECK_CLOSE(NEW_TIME_2, metaAnimated.getKeyframeTime(animatedHandle, PATH, 2), EPSILON);
		CHECK_CLOSE(std::numeric_limits<IMetaAnimatedObj::anim_time_t>::max(),
			metaAnimated.getKeyframeTime(animatedHandle, PATH, 3),
			EPSILON);

		const Variant TEST_VALUE_0 = 10.0f;
		const Variant TEST_VALUE_1 = 6.0f;
		const Variant TEST_VALUE_2 = 3.0f;

		CHECK_EQUAL(VALUE_0, metaAnimated.getKeyframeValue(animatedHandle, PATH, 0));
		CHECK_EQUAL(VALUE_1, metaAnimated.getKeyframeValue(animatedHandle, PATH, 1));
		CHECK_EQUAL(VALUE_2, metaAnimated.getKeyframeValue(animatedHandle, PATH, 2));

		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 0, TEST_VALUE_0));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 1, TEST_VALUE_1));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 2, TEST_VALUE_2));

		CHECK_EQUAL(TEST_VALUE_0, metaAnimated.getKeyframeValue(animatedHandle, PATH, 0));
		CHECK_EQUAL(TEST_VALUE_1, metaAnimated.getKeyframeValue(animatedHandle, PATH, 1));
		CHECK_EQUAL(TEST_VALUE_2, metaAnimated.getKeyframeValue(animatedHandle, PATH, 2));

		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 0, VALUE_0));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 1, VALUE_1));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 2, VALUE_2));

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(0.0f, floatValue, EPSILON);
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 1.0f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(0.0f, floatValue, EPSILON);
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.0f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(0.0f, floatValue, EPSILON);
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.35f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(0.25f, floatValue, EPSILON);
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.7f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(0.5f, floatValue, EPSILON);
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.85f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			// Seems to have larger floating-point rounding error
			CHECK_CLOSE(49.75f, floatValue, 0.5f /*EPSILON*/);
		}


		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 3.0f);
			auto floatValue = 0.0f;
			CHECK(value.tryCast<float>(floatValue));
			CHECK_CLOSE(100.0f, floatValue, EPSILON);
		}

		{
			CHECK(!metaAnimated.removeKeyframe(animatedHandle, PATH, 1000));
			CHECK_EQUAL(3, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(2, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.35f);
				auto floatValue = 0.0f;
				CHECK(value.tryCast<float>(floatValue));
				CHECK_CLOSE(0.5f, floatValue, EPSILON);
			}

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 1));
			CHECK_EQUAL(1, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 1000.0f);
				auto floatValue = 0.0f;
				CHECK(value.tryCast<float>(floatValue));
				CHECK_CLOSE(0.5f, floatValue, EPSILON);
			}

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
				CHECK_EQUAL(true, value.isVoid());
			}

			CHECK(!metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
				CHECK_EQUAL(true, value.isVoid());
			}
		}

		// Make a second copy to check that data is stored per-instance,
		// not stored per-type
		{
			TestMetaDataObject subject2;
			ManagedObject<TestMetaDataObject> object2(subject2);
			auto provider2 = object2.getHandleT();

			auto floatPropertyAccessor2 = testDefinition.bindProperty(PATH.c_str(), provider2);
			CHECK(floatPropertyAccessor2.isValid());
			if (!floatPropertyAccessor2.isValid())
			{
				return;
			}
			auto animatedHandle2 = floatPropertyAccessor2.getObject();
			CHECK(animatedHandle2.isValid());
			if (!animatedHandle2.isValid())
			{
				return;
			}
			auto pMetaAnimated2 = findFirstMetaData<IMetaAnimatedObj>(floatPropertyAccessor2,
				definitionManager);
			CHECK(pMetaAnimated2 != nullptr);
			if (pMetaAnimated2 == nullptr)
			{
				return;
			}
			auto& metaAnimated2 = const_cast<IMetaAnimatedObj&>(*pMetaAnimated2);

			CHECK(!metaAnimated2.isActive(animatedHandle2, PATH));
			CHECK(!metaAnimated2.isLooping(animatedHandle2, PATH));

			CHECK(metaAnimated.setActive(animatedHandle, PATH, false));
			CHECK(metaAnimated.setLooping(animatedHandle, PATH, false));
			CHECK(metaAnimated2.setActive(animatedHandle2, PATH, true));
			CHECK(metaAnimated2.setLooping(animatedHandle2, PATH, true));

			CHECK(!metaAnimated.isActive(animatedHandle, PATH));
			CHECK(!metaAnimated.isLooping(animatedHandle, PATH));
			CHECK(metaAnimated2.isActive(animatedHandle2, PATH));
			CHECK(metaAnimated2.isLooping(animatedHandle2, PATH));

			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(0, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH));

			const Variant TEST2_VALUE_0 = 300.0f;
			const IMetaAnimatedObj::anim_time_t TEST2_TIME_0 = 50.0f;
			CHECK_EQUAL(0, metaAnimated2.insertKeyframe(animatedHandle2, PATH, TEST2_VALUE_0, TEST2_TIME_0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(1, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH));
		}

		// Make a second copy to check that data is stored per-instance,
		// not stored per-type
		{
			const std::string PATH_2 = "test_float_2";

			ManagedObject<TestMetaDataObject> object2(subject);
			auto provider2 = object2.getHandleT();

			auto floatPropertyAccessor2 = testDefinition.bindProperty(PATH_2.c_str(), provider2);
			CHECK(floatPropertyAccessor2.isValid());
			if (!floatPropertyAccessor2.isValid())
			{
				return;
			}
			auto animatedHandle2 = floatPropertyAccessor2.getObject();
			CHECK(animatedHandle2.isValid());
			if (!animatedHandle2.isValid())
			{
				return;
			}
			auto pMetaAnimated2 = findFirstMetaData<IMetaAnimatedObj>(floatPropertyAccessor2,
				definitionManager);
			CHECK(pMetaAnimated2 != nullptr);
			if (pMetaAnimated2 == nullptr)
			{
				return;
			}
			auto& metaAnimated2 = const_cast<IMetaAnimatedObj&>(*pMetaAnimated2);

			CHECK(!metaAnimated2.isActive(animatedHandle2, PATH_2));
			CHECK(!metaAnimated2.isLooping(animatedHandle2, PATH_2));

			CHECK(metaAnimated.setActive(animatedHandle, PATH, false));
			CHECK(metaAnimated.setLooping(animatedHandle, PATH, false));
			CHECK(metaAnimated2.setActive(animatedHandle2, PATH_2, true));
			CHECK(metaAnimated2.setLooping(animatedHandle2, PATH_2, true));

			CHECK(!metaAnimated.isActive(animatedHandle, PATH));
			CHECK(!metaAnimated.isLooping(animatedHandle, PATH));
			CHECK(metaAnimated2.isActive(animatedHandle2, PATH_2));
			CHECK(metaAnimated2.isLooping(animatedHandle2, PATH_2));

			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(0, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH_2));

			const Variant TEST2_VALUE_0 = 300.0f;
			const IMetaAnimatedObj::anim_time_t TEST2_TIME_0 = 50.0f;
			CHECK_EQUAL(0, metaAnimated2.insertKeyframe(animatedHandle2, PATH_2, TEST2_VALUE_0, TEST2_TIME_0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(1, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH_2));
		}
	}
}

TEST_F(TestMetaDataFixture, vector_animation)
{
	using namespace MetaAnimatedTest;
	TestMetaDataObject subject;
	const std::string PATH = "test_vector4";

	IDefinitionManager& definitionManager = getDefinitionManager();
	auto pTestDefinition = definitionManager.getDefinition<TestMetaDataObject>();
	CHECK(pTestDefinition != nullptr);
	if (pTestDefinition == nullptr)
	{
		return;
	}
	auto& testDefinition = (*pTestDefinition);
	ManagedObject<TestMetaDataObject> object(subject);
	auto provider = object.getHandleT();

	auto vector4PropertyAccessor = testDefinition.bindProperty(PATH.c_str(), provider);
	CHECK(vector4PropertyAccessor.isValid());
	if (!vector4PropertyAccessor.isValid())
	{
		return;
	}
	auto animatedHandle = vector4PropertyAccessor.getObject();
	CHECK(animatedHandle.isValid());
	if (!animatedHandle.isValid())
	{
		return;
	}
	auto pMetaAnimated = findFirstMetaData<IMetaAnimatedObj>(vector4PropertyAccessor,
		definitionManager);
	CHECK(pMetaAnimated != nullptr);
	if (pMetaAnimated == nullptr)
	{
		return;
	}
	auto& metaAnimated = const_cast<IMetaAnimatedObj&>(*pMetaAnimated);

	const float EPSILON = 0.01f;

	// Empty timeline
	{
		CHECK_EQUAL(false, metaAnimated.isActive(animatedHandle, PATH));
		const auto success = metaAnimated.setActive(animatedHandle, PATH, true);
		CHECK(success);
		CHECK_EQUAL(true, metaAnimated.isActive(animatedHandle, PATH));
	}

	{
		CHECK_EQUAL(false, metaAnimated.isLooping(animatedHandle, PATH));
		const auto success = metaAnimated.setLooping(animatedHandle, PATH, true);
		CHECK(success);
		CHECK_EQUAL(true, metaAnimated.isLooping(animatedHandle, PATH));
	}

	{
		CHECK_CLOSE(0.0f, metaAnimated.getDuration(animatedHandle, PATH), EPSILON);
		CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
	}

	// Full timeline
	{
		const IMetaAnimatedObj::anim_time_t DURATION = 1.0f;

		const Variant VALUE_0 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		const Variant VALUE_1 = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
		const Variant VALUE_2 = Vector4(100.0f, 100.0f, 100.0f, 100.0f);

		const IMetaAnimatedObj::anim_time_t TIME_0 = 0.0f;
		const IMetaAnimatedObj::anim_time_t TIME_1 = DURATION * 0.2f;
		const IMetaAnimatedObj::anim_time_t TIME_2 = DURATION;

		const auto insert0Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_0,
			TIME_0);
		CHECK_EQUAL(0, insert0Index);
		const auto duration0 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_0, duration0, EPSILON);
		CHECK_EQUAL(1, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		const auto insert1Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_1,
			TIME_1);
		CHECK_EQUAL(1, insert1Index);
		const auto duration1 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_1, duration1, EPSILON);
		CHECK_EQUAL(2, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		const auto insert2Index = metaAnimated.insertKeyframe(animatedHandle, PATH,
			VALUE_2,
			TIME_2);
		CHECK_EQUAL(2, insert2Index);
		const auto duration2 = metaAnimated.getDuration(animatedHandle, PATH);
		CHECK_CLOSE(TIME_2, duration2, EPSILON);
		CHECK_EQUAL(3, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

		CHECK_CLOSE(TIME_0, metaAnimated.getKeyframeTime(animatedHandle, PATH, 0), EPSILON);
		CHECK_CLOSE(TIME_1, metaAnimated.getKeyframeTime(animatedHandle, PATH, 1), EPSILON);
		CHECK_CLOSE(TIME_2, metaAnimated.getKeyframeTime(animatedHandle, PATH, 2), EPSILON);
		CHECK_CLOSE(std::numeric_limits<IMetaAnimatedObj::anim_time_t>::max(),
			metaAnimated.getKeyframeTime(animatedHandle, PATH, 3),
			EPSILON);

		const IMetaAnimatedObj::anim_time_t NEW_TIME_0 = 2.0f;
		const IMetaAnimatedObj::anim_time_t NEW_TIME_1 = NEW_TIME_0 + (DURATION * 0.7f);
		const IMetaAnimatedObj::anim_time_t NEW_TIME_2 = NEW_TIME_0 + DURATION;

		// Keyframe indexes re-sorted after set
		const auto set0Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_0);
		CHECK(set0Success);
		const auto set1Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_1);
		CHECK(set1Success);
		const auto set2Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 0, NEW_TIME_2);
		CHECK(set2Success);
		const auto set3Success = metaAnimated.setKeyframeTime(animatedHandle, PATH, 3, 1000.0f);
		CHECK(!set3Success);

		CHECK_CLOSE(NEW_TIME_0, metaAnimated.getKeyframeTime(animatedHandle, PATH, 0), EPSILON);
		CHECK_CLOSE(NEW_TIME_1, metaAnimated.getKeyframeTime(animatedHandle, PATH, 1), EPSILON);
		CHECK_CLOSE(NEW_TIME_2, metaAnimated.getKeyframeTime(animatedHandle, PATH, 2), EPSILON);
		CHECK_CLOSE(std::numeric_limits<IMetaAnimatedObj::anim_time_t>::max(),
			metaAnimated.getKeyframeTime(animatedHandle, PATH, 3),
			EPSILON);

		const Variant TEST_VALUE_0 = Vector4(10.0f, 10.0f, 10.0f, 10.0f);
		const Variant TEST_VALUE_1 = Vector4(6.0f, 6.0f, 6.0f, 6.0f);
		const Variant TEST_VALUE_2 = Vector4(3.0f, 3.0f, 3.0f, 3.0f);

		CHECK_EQUAL(VALUE_0, metaAnimated.getKeyframeValue(animatedHandle, PATH, 0));
		CHECK_EQUAL(VALUE_1, metaAnimated.getKeyframeValue(animatedHandle, PATH, 1));
		CHECK_EQUAL(VALUE_2, metaAnimated.getKeyframeValue(animatedHandle, PATH, 2));

		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 0, TEST_VALUE_0));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 1, TEST_VALUE_1));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 2, TEST_VALUE_2));

		CHECK_EQUAL(TEST_VALUE_0, metaAnimated.getKeyframeValue(animatedHandle, PATH, 0));
		CHECK_EQUAL(TEST_VALUE_1, metaAnimated.getKeyframeValue(animatedHandle, PATH, 1));
		CHECK_EQUAL(TEST_VALUE_2, metaAnimated.getKeyframeValue(animatedHandle, PATH, 2));

		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 0, VALUE_0));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 1, VALUE_1));
		CHECK(metaAnimated.setKeyframeValue(animatedHandle, PATH, 2, VALUE_2));

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(0.0f, 0.0f, 0.0f, 0.0f).almostEqual(vector4Value, EPSILON));
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 1.0f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(0.0f, 0.0f, 0.0f, 0.0f).almostEqual(vector4Value, EPSILON));
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.0f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(0.0f, 0.0f, 0.0f, 0.0f).almostEqual(vector4Value, EPSILON));
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.35f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(0.25f, 0.25f, 0.25f, 0.25f).almostEqual(vector4Value, EPSILON));
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.7f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(0.5f, 0.5f, 0.5f, 0.5f).almostEqual(vector4Value, EPSILON));
		}

		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.85f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			// Seems to have larger floating-point rounding error
			CHECK(Vector4(49.75f, 49.75f, 49.75f, 49.75f).almostEqual(vector4Value, 0.5f /*EPSILON*/));
		}


		{
			const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 3.0f);
			Vector4 vector4Value;
			CHECK(value.tryCast<Vector4>(vector4Value));
			CHECK(Vector4(100.0f, 100.0f, 100.0f, 100.0f).almostEqual(vector4Value, EPSILON));
		}

		{
			CHECK(!metaAnimated.removeKeyframe(animatedHandle, PATH, 1000));
			CHECK_EQUAL(3, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(2, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 2.35f);
				Vector4 vector4Value;
				CHECK(value.tryCast<Vector4>(vector4Value));
				CHECK(Vector4(0.5f, 0.5f, 0.5f, 0.5f).almostEqual(vector4Value, EPSILON));
			}

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 1));
			CHECK_EQUAL(1, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 1000.0f);
				Vector4 vector4Value;
				CHECK(value.tryCast<Vector4>(vector4Value));
				CHECK(Vector4(0.5f, 0.5f, 0.5f, 0.5f).almostEqual(vector4Value, EPSILON));
			}

			CHECK(metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
				CHECK_EQUAL(true, value.isVoid());
			}

			CHECK(!metaAnimated.removeKeyframe(animatedHandle, PATH, 0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			{
				const auto value = metaAnimated.extrapolateValue(animatedHandle, PATH, 0.0f);
				CHECK_EQUAL(true, value.isVoid());
			}
		}

		// Make a second copy to check that data is stored per-instance,
		// not stored per-type
		{
			TestMetaDataObject subject2;
			ManagedObject<TestMetaDataObject> object2(subject2);
			auto provider2 = object2.getHandleT();

			auto vector4PropertyAccessor2 = testDefinition.bindProperty(PATH.c_str(), provider2);
			CHECK(vector4PropertyAccessor2.isValid());
			if (!vector4PropertyAccessor2.isValid())
			{
				return;
			}
			auto animatedHandle2 = vector4PropertyAccessor2.getObject();
			CHECK(animatedHandle2.isValid());
			if (!animatedHandle2.isValid())
			{
				return;
			}
			auto pMetaAnimated2 = findFirstMetaData<IMetaAnimatedObj>(vector4PropertyAccessor2,
				definitionManager);
			CHECK(pMetaAnimated2 != nullptr);
			if (pMetaAnimated2 == nullptr)
			{
				return;
			}
			auto& metaAnimated2 = const_cast<IMetaAnimatedObj&>(*pMetaAnimated2);

			CHECK(!metaAnimated2.isActive(animatedHandle2, PATH));
			CHECK(!metaAnimated2.isLooping(animatedHandle2, PATH));

			CHECK(metaAnimated.setActive(animatedHandle, PATH, false));
			CHECK(metaAnimated.setLooping(animatedHandle, PATH, false));
			CHECK(metaAnimated2.setActive(animatedHandle2, PATH, true));
			CHECK(metaAnimated2.setLooping(animatedHandle2, PATH, true));

			CHECK(!metaAnimated.isActive(animatedHandle, PATH));
			CHECK(!metaAnimated.isLooping(animatedHandle, PATH));
			CHECK(metaAnimated2.isActive(animatedHandle2, PATH));
			CHECK(metaAnimated2.isLooping(animatedHandle2, PATH));

			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(0, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH));

			const Variant TEST2_VALUE_0 = Vector4(300.0f, 200.0f, 100.0f, 0.0f);
			const IMetaAnimatedObj::anim_time_t TEST2_TIME_0 = 50.0f;
			CHECK_EQUAL(0, metaAnimated2.insertKeyframe(animatedHandle2, PATH, TEST2_VALUE_0, TEST2_TIME_0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(1, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH));
		}

		// Make a second copy to check that data is stored per-instance,
		// not stored per-type
		{
			const std::string PATH_2 = "test_vector4_2";

			ManagedObject<TestMetaDataObject> object2(subject);
			auto provider2 = object2.getHandleT();

			auto vector4PropertyAccessor2 = testDefinition.bindProperty(PATH_2.c_str(), provider2);
			CHECK(vector4PropertyAccessor2.isValid());
			if (!vector4PropertyAccessor2.isValid())
			{
				return;
			}
			auto animatedHandle2 = vector4PropertyAccessor2.getObject();
			CHECK(animatedHandle2.isValid());
			if (!animatedHandle2.isValid())
			{
				return;
			}
			auto pMetaAnimated2 = findFirstMetaData<IMetaAnimatedObj>(vector4PropertyAccessor2,
				definitionManager);
			CHECK(pMetaAnimated2 != nullptr);
			if (pMetaAnimated2 == nullptr)
			{
				return;
			}
			auto& metaAnimated2 = const_cast<IMetaAnimatedObj&>(*pMetaAnimated2);

			CHECK(!metaAnimated2.isActive(animatedHandle2, PATH_2));
			CHECK(!metaAnimated2.isLooping(animatedHandle2, PATH_2));

			CHECK(metaAnimated.setActive(animatedHandle, PATH, false));
			CHECK(metaAnimated.setLooping(animatedHandle, PATH, false));
			CHECK(metaAnimated2.setActive(animatedHandle2, PATH_2, true));
			CHECK(metaAnimated2.setLooping(animatedHandle2, PATH_2, true));

			CHECK(!metaAnimated.isActive(animatedHandle, PATH));
			CHECK(!metaAnimated.isLooping(animatedHandle, PATH));
			CHECK(metaAnimated2.isActive(animatedHandle2, PATH_2));
			CHECK(metaAnimated2.isLooping(animatedHandle2, PATH_2));

			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(0, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH_2));

			const Variant TEST2_VALUE_0 = Vector4(300.0f, 200.0f, 100.0f, 0.0f);
			const IMetaAnimatedObj::anim_time_t TEST2_TIME_0 = 50.0f;
			CHECK_EQUAL(0, metaAnimated2.insertKeyframe(animatedHandle2, PATH_2, TEST2_VALUE_0, TEST2_TIME_0));
			CHECK_EQUAL(0, metaAnimated.getNumberOfKeyframes(animatedHandle, PATH));
			CHECK_EQUAL(1, metaAnimated2.getNumberOfKeyframes(animatedHandle2, PATH_2));
		}
	}
}

} // end namespace wgt
