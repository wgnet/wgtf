#include "pch.hpp"

#include "test_meta_animated_object.hpp"
#include "core_common/assert.hpp"
#include "wg_types/hash_utilities.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <vector>

namespace wgt
{

class TestMetaAnimatedObj::Implementation
{
public:
	Implementation();

	struct AnimationData
	{
		AnimationData();
		bool isActive_;
		bool isLooping_;
		struct Keyframe
		{
			IMetaAnimatedObj::anim_time_t time_;
			Variant value_;
		};
		std::vector<Keyframe> keyframes_;
	};

	struct KeyframeComparator
	{
		bool operator()(const AnimationData::Keyframe& a,
			const AnimationData::Keyframe& b) const;
	};

	typedef uint64_t ObjectId;
	std::map<ObjectId, AnimationData> dataStore_;

	static ObjectId computeId(const ObjectHandle& handle, const std::string& path);

	/**
	 *	Based on @see Curve undo/redo implementation.
	 *	But with different return value to accommodate the return value of
	 *	insertKeyframe.
	 *	
	 *	This is required to undo insert/remove and all set functions.
	 *	
	 *	Since set functions require arguments, they must be exposed as methods
	 *	instead of properties, so the reflection system cannot automatically
	 *	perform a undo/redo.
	 */
	typedef std::function<size_t()> ModificationFunction;
	size_t pushModification(ModificationFunction&& executeFunc,
		ModificationFunction&& undoFunc);

	struct AnimationModification
	{
		AnimationModification(ModificationFunction&& executeFunc,
			ModificationFunction&& undoFunc)
		    : undo_(std::move(undoFunc)), redo_(std::move(executeFunc))
		{
		}

		ModificationFunction undo_;
		ModificationFunction redo_;
	};
	std::vector<AnimationModification> modificationStack_;
	size_t currentState_;

};

TestMetaAnimatedObj::Implementation::Implementation()
	: currentState_(-1)
{
}

TestMetaAnimatedObj::Implementation::AnimationData::AnimationData()
	: isActive_(false)
	, isLooping_(false)
{
}

bool TestMetaAnimatedObj::Implementation::KeyframeComparator::operator()(
	const TestMetaAnimatedObj::Implementation::AnimationData::Keyframe& a,
	const TestMetaAnimatedObj::Implementation::AnimationData::Keyframe& b) const
{
	return (a.time_ < b.time_);
}

TestMetaAnimatedObj::TestMetaAnimatedObj()
	: IMetaAnimatedObj()
	, impl_(new Implementation())
{
}

TestMetaAnimatedObj::~TestMetaAnimatedObj()
{
}

bool TestMetaAnimatedObj::isActive(const ObjectHandle& handle,
	const std::string& path) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	return impl_->dataStore_[id].isActive_;
}

bool TestMetaAnimatedObj::setActive(ObjectHandle& handle,
	const std::string& path,
	bool active) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	auto executeFunc = [&animationData, active]() -> size_t {
		animationData.isActive_ = active;
		return 1;
	};
	auto undoFunc = [&animationData, active]() -> size_t {
		animationData.isActive_ = !active;
		return 1;
	};
	return (impl_->pushModification(std::move(executeFunc), std::move(undoFunc)) > 0);
}

bool TestMetaAnimatedObj::isLooping(const ObjectHandle& handle,
	const std::string& path) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	return impl_->dataStore_[id].isLooping_;
}

bool TestMetaAnimatedObj::setLooping(ObjectHandle& handle,
	const std::string& path,
	bool looping) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	auto executeFunc = [&animationData, looping]() -> size_t {
		animationData.isLooping_ = looping;
		return 1;
	};
	auto undoFunc = [&animationData, looping]() -> size_t {
		animationData.isLooping_ = !looping;
		return 1;
	};
	return (impl_->pushModification(std::move(executeFunc), std::move(undoFunc)) > 0);
}

TestMetaAnimatedObj::anim_time_t TestMetaAnimatedObj::getDuration(
	const ObjectHandle& handle,
	const std::string& path) /* override */
{
	// Assumes keyframes are sorted
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	if (animationData.keyframes_.empty())
	{
		return 0.0f;
	}
	return animationData.keyframes_.crbegin()->time_;
}

size_t TestMetaAnimatedObj::getNumberOfKeyframes(
	const ObjectHandle& handle,
	const std::string& path) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	return impl_->dataStore_[id].keyframes_.size();
}

size_t TestMetaAnimatedObj::insertKeyframe(ObjectHandle& handle,
	const std::string& path,
	const Variant& value,
	anim_time_t time) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];

	while (impl_->currentState_ < impl_->modificationStack_.size() - 1)
	{
		impl_->modificationStack_.pop_back();
	}
	auto executeFunc = [&animationData, value, time]() -> size_t {
		// Sorted insert
		Implementation::AnimationData::Keyframe keyframe{time, value};
		const auto itr = std::upper_bound(animationData.keyframes_.cbegin(),
			animationData.keyframes_.cend(),
			keyframe,
			Implementation::KeyframeComparator());
		const auto result = animationData.keyframes_.emplace(itr, std::move(keyframe));
		return std::distance(std::begin(animationData.keyframes_), result);
	};
	// Need result for undo
	const auto result = executeFunc();
	auto undoFunc = [&animationData, value, result]() -> size_t {
		const auto keyframeIndex = result;
		if (keyframeIndex >= animationData.keyframes_.size())
		{
			return 0;
		}
		const auto itr = animationData.keyframes_.cbegin() + keyframeIndex;
		animationData.keyframes_.erase(itr);
		return 1;
	};
	++impl_->currentState_;
	impl_->modificationStack_.emplace_back(std::move(executeFunc), std::move(undoFunc));
	return result;
}

bool TestMetaAnimatedObj::removeKeyframe(ObjectHandle& handle,
	const std::string& path,
	size_t keyframeIndex) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	if (keyframeIndex >= animationData.keyframes_.size())
	{
		return (impl_->pushModification([]() -> size_t { return 0; },
			[]() -> size_t { return 0; }) > 0);
	}

	auto executeFunc = [&animationData, keyframeIndex]() -> size_t {
		const auto itr = animationData.keyframes_.cbegin() + keyframeIndex;
		animationData.keyframes_.erase(itr);
		return 1;
	};

	const Implementation::AnimationData::Keyframe keyframeCopy =
		animationData.keyframes_[keyframeIndex];
	auto undoFunc = [&animationData, keyframeCopy]() -> size_t {

		// Sorted insert
		const auto itr = std::upper_bound(animationData.keyframes_.cbegin(),
			animationData.keyframes_.cend(),
			keyframeCopy,
			Implementation::KeyframeComparator());
		const auto result = animationData.keyframes_.insert(itr, keyframeCopy);
		return (result != animationData.keyframes_.cend()) ? 1 : 0;
	};
	return (impl_->pushModification(std::move(executeFunc), std::move(undoFunc)) > 0);
}


TestMetaAnimatedObj::anim_time_t TestMetaAnimatedObj::getKeyframeTime(
	const ObjectHandle& handle,
	const std::string& path,
	size_t keyframeIndex) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	const auto& animationData = impl_->dataStore_[id];
	if (keyframeIndex >= animationData.keyframes_.size())
	{
		return std::numeric_limits<anim_time_t>::max();
	}
	return animationData.keyframes_[keyframeIndex].time_;
}

bool TestMetaAnimatedObj::setKeyframeTime(ObjectHandle& handle,
	const std::string& path,
	size_t keyframeIndex,
	anim_time_t time) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	if (keyframeIndex >= animationData.keyframes_.size())
	{
		return (impl_->pushModification([]() -> size_t { return 0; },
			[]() -> size_t { return 0; }) > 0);
	}
	auto& keyframe = animationData.keyframes_[keyframeIndex];
	auto executeFunc = [&animationData, &keyframe, time]() {
		keyframe.time_ = time;
		// Note: unstable sort, invalidates all keyframe indexes
		std::sort(std::begin(animationData.keyframes_),
			std::end(animationData.keyframes_),
			Implementation::KeyframeComparator());
		return 1;
	};

	const auto oldTime = keyframe.time_;
	auto undoFunc = [&animationData, &keyframe, oldTime]() {
		keyframe.time_ = oldTime;
		// Note: unstable sort, invalidates all keyframe indexes
		std::sort(std::begin(animationData.keyframes_),
			std::end(animationData.keyframes_),
			Implementation::KeyframeComparator());
		return 1;
	};
	return (impl_->pushModification(std::move(executeFunc), std::move(undoFunc)) > 0);
}

Variant TestMetaAnimatedObj::getKeyframeValue(const ObjectHandle& handle,
	const std::string& path,
	size_t keyframeIndex) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	const auto& animationData = impl_->dataStore_[id];
	if (keyframeIndex >= animationData.keyframes_.size())
	{
		return Variant();
	}
	return animationData.keyframes_[keyframeIndex].value_;
}

bool TestMetaAnimatedObj::setKeyframeValue(const ObjectHandle& handle,
	const std::string& path,
	size_t keyframeIndex,
	const Variant& value) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];
	if (keyframeIndex >= animationData.keyframes_.size())
	{
		return (impl_->pushModification([]() -> size_t { return 0; },
			[]() -> size_t { return 0; }) > 0);
	}
	auto& keyframe = animationData.keyframes_[keyframeIndex];
	auto executeFunc = [&animationData, &keyframe, value]() {
		keyframe.value_ = value;
		return 1;
	};

	const auto oldValue = keyframe.value_;
	auto undoFunc = [&animationData, &keyframe, oldValue]() {
		keyframe.value_ = oldValue;
		return 1;
	};
	return (impl_->pushModification(std::move(executeFunc), std::move(undoFunc)) > 0);
}

Variant TestMetaAnimatedObj::extrapolateValue(const ObjectHandle& handle,
	const std::string& path,
	TestMetaAnimatedObj::anim_time_t time) /* override */
{
	const Implementation::ObjectId id = Implementation::computeId(handle, path);
	auto& animationData = impl_->dataStore_[id];

	if (animationData.keyframes_.empty())
	{
		return Variant();
	}

	if (animationData.keyframes_.size() == 1)
	{
		return animationData.keyframes_.cbegin()->value_;
	}

	const Implementation::AnimationData::Keyframe dummyKeyframe{time, Variant()};
	const auto upperItr = std::upper_bound(animationData.keyframes_.cbegin(),
		animationData.keyframes_.cend(),
		dummyKeyframe,
		Implementation::KeyframeComparator());

	if (upperItr == animationData.keyframes_.cbegin())
	{
		return upperItr->value_;
	}
	else if (upperItr == animationData.keyframes_.cend())
	{
		return animationData.keyframes_.crbegin()->value_;
	}
	const auto lowerItr = upperItr - 1;

	const auto& lowerFrame = (*lowerItr);
	const auto& higherFrame = (*upperItr);
	TF_ASSERT(lowerFrame.time_ <= higherFrame.time_);
	TF_ASSERT(lowerFrame.time_ <= time);
	TF_ASSERT(time <= higherFrame.time_);

	const auto diffTime = (time - lowerFrame.time_) / (higherFrame.time_ - lowerFrame.time_);
	return this->extrapolateValue(lowerFrame.value_, higherFrame.value_, diffTime);
}

void TestMetaAnimatedObj::undo(const ObjectHandle& handle, Variant variant)
{
	impl_->modificationStack_[impl_->currentState_].undo_();
	--(impl_->currentState_);
}

void TestMetaAnimatedObj::redo(const ObjectHandle& handle, Variant variant)
{
	++(impl_->currentState_);
	impl_->modificationStack_[impl_->currentState_].redo_();
}

TestMetaAnimatedObj::Implementation::ObjectId
TestMetaAnimatedObj::Implementation::computeId(const ObjectHandle& handle,
	const std::string& path)
{
	auto totalHash = HashUtilities::compute(reinterpret_cast<size_t>(handle.data()));
	const auto pathHash = HashUtilities::compute(path);
	HashUtilities::directCombine(totalHash, pathHash);
	return totalHash;
}

size_t TestMetaAnimatedObj::Implementation::pushModification(
	TestMetaAnimatedObj::Implementation::ModificationFunction&& executeFunc,
	TestMetaAnimatedObj::Implementation::ModificationFunction&& undoFunc)
{
	while (currentState_ < modificationStack_.size() - 1)
	{
		modificationStack_.pop_back();
	}
	++currentState_;
	const auto result = executeFunc();
	modificationStack_.emplace_back(std::move(executeFunc), std::move(undoFunc));
	return result;
}

} // end namespace wgt

