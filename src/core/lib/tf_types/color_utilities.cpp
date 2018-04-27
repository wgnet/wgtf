#include "color_utilities.hpp"
#include <algorithm>
#include <string>
#include <cmath>

namespace wgt
{
namespace ColorUtilities
{
const float GAMMA = 2.2;

Vector4 makeColor(int r, int g, int b, int a)
{
	return clamp(Vector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
}

Vector4 makeHDRColor(int r, int g, int b, float intensity)
{
	return setExposure(makeColor(r, g, b), intensity);
}

Vector4 setExposure(const Vector4& v, float intensity)
{
	return v * std::pow(2.0f, intensity);
}

Vector4 clamp(const Vector4& v)
{
	return Vector4(std::max(std::min(1.0f, v.x), 0.0f), std::max(std::min(1.0f, v.y), 0.0f),
	               std::max(std::min(1.0f, v.z), 0.0f), std::max(std::min(1.0f, v.w), 0.0f));
}

Vector4 reinhardTonemap(const Vector4& v, float luminanceWhite)
{
	const float EPSILON = 0.00000000001f;
	const float lum = luminance(v);
	if (lum < EPSILON)
	{
		return v;
	}

	const float tmLum = (lum * (1.0f + (lum / (luminanceWhite * luminanceWhite)))) / (1.0f + lum);
	const float scale = tmLum / lum;

	return clamp(Vector4(std::pow(v.x * scale, 1.0f / GAMMA), std::pow(v.y * scale, 1.0f / GAMMA),
	                     std::pow(v.z * scale, 1.0f / GAMMA), std::pow(v.w * scale, 1.0f / GAMMA)));
}

float luminance(const Vector4& v)
{
	return 0.2126f * v.x + 0.7152f * v.y + 0.0722f * v.z;
}
}
} // end namespace wgt