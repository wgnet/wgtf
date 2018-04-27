#pragma once

#include "core_reflection/metadata/meta_impl.hpp"

#include <memory>

namespace wgt
{

/**
 *	Test implementation of animated properties.
 */
class TestMetaAnimatedObj : public IMetaAnimatedObj
{
public:
	TestMetaAnimatedObj();
	virtual ~TestMetaAnimatedObj();

	virtual bool isActive(const ObjectHandle& handle,
		const std::string& path) override;
	virtual bool setActive(ObjectHandle& handle,
		const std::string& path,
		bool active) override;

	virtual bool isLooping(const ObjectHandle& handle,
		const std::string& path) override;
	virtual bool setLooping(ObjectHandle& handle,
		const std::string& path,
		bool looping) override;

	virtual anim_time_t getDuration(const ObjectHandle& handle,
		const std::string& path) override;
	virtual size_t getNumberOfKeyframes(const ObjectHandle& handle,
		const std::string& path) override;
	virtual size_t insertKeyframe(ObjectHandle& handle,
		const std::string& path,
		const Variant& value,
		anim_time_t time) override;
	virtual bool removeKeyframe(ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) override;

	virtual anim_time_t getKeyframeTime(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) override;
	virtual bool setKeyframeTime(ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex,
		anim_time_t time) override;

	virtual Variant getKeyframeValue(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex) override;
	virtual bool setKeyframeValue(const ObjectHandle& handle,
		const std::string& path,
		size_t keyframeIndex,
		const Variant& value) override;

	virtual Variant extrapolateValue(const ObjectHandle& handle,
		const std::string& path,
		anim_time_t time) override;

	void undo(const ObjectHandle& handle, Variant variant);
	void redo(const ObjectHandle& handle, Variant variant);

protected:
	/// Implement for particular Variant types; int, float, vector4 etc
	virtual Variant extrapolateValue(const Variant& lowerVariant,
		const Variant& higherVariant,
		float percent) = 0;

private:
	class Implementation;
	std::unique_ptr<Implementation> impl_;
};

} // end namespace wgt

