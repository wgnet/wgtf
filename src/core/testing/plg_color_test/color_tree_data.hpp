#pragma once

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "wg_types/color_utilities.hpp"

namespace wgt
{
class ColorTreeData
{
	DECLARE_REFLECTED

public:
	ColorTreeData()
	    : color3_(213.0f, 0.0f, 0.0f), color4_(54.0f, 211.0f, 46.0f, 255.0f),
	      colorHDR_(ColorUtilities::makeHDRColor(0, 59, 180, -1.0f))
	{
	}

	void setColor3(const Vector3& color)
	{
		color3_.x = color.x;
		color3_.y = color.y;
		color3_.z = color.z;
	}

	void getColor3(Vector3* color) const
	{
		color->x = color3_.x;
		color->y = color3_.y;
		color->z = color3_.z;
	}

	void setColor4(const Vector4& color)
	{
		color4_.x = color.x;
		color4_.y = color.y;
		color4_.z = color.z;
		color4_.w = color.w;
	}

	void getColor4(Vector4* color) const
	{
		color->x = color4_.x;
		color->y = color4_.y;
		color->z = color4_.z;
		color->w = color4_.w;
	}

	void setHDRColor(const Vector4& color)
	{
		colorHDR_.x = color.x;
		colorHDR_.y = color.y;
		colorHDR_.z = color.z;
		colorHDR_.w = color.w;
	}

	void getHDRColor(Vector4* color) const
	{
		color->x = colorHDR_.x;
		color->y = colorHDR_.y;
		color->z = colorHDR_.z;
		color->w = colorHDR_.w;
	}

	void setLuminanceWhite(const double& v)
	{
		MetaHDRColorReinhardTonemapObj::setLuminanceWhite(float(v));
	}

	void getLuminanceWhite(double* v) const
	{
		*v = MetaHDRColorReinhardTonemapObj::getLuminanceWhite();
	}

	static Vector4 tonemapOperator(const Vector4& v)
	{
		return ColorUtilities::reinhardTonemap(v, MetaHDRColorReinhardTonemapObj::getLuminanceWhite());
	}

private:
	Vector3 color3_;
	Vector4 color4_;
	Vector4 colorHDR_;
};
}