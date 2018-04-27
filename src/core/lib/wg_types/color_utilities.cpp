#include "color_utilities.hpp"
#include <algorithm>
#include <string>
#include <cmath>

namespace wgt
{
namespace ColorUtilities
{
const float GAMMA = 2.2;

Int4 makeColor(const Vector4& color)
{
	const auto clampedColor = clamp(color);
	return Int4
	{
		int(clampedColor.x * 255.0f),
		int(clampedColor.y * 255.0f),
		int(clampedColor.z * 255.0f),
		int(clampedColor.w * 255.0f)
	};
}

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

Vector4 convertKelvinToRGB(int kelvin)
{
	Vector4 rgb(0.0f, 0.0f, 0.0f, 1.0f);
	const float k = std::max(0.0f, (float)kelvin);

	if (k <= 6500.0f)
	{
		rgb.x = 1.0f;
	}
	else
	{
		rgb.x = 108180.0f * std::pow((k - 2080.2f), -1.422f) + 0.29533f;
	}

	if (k < 6600.0f)
	{
		rgb.y = -0.03021f * std::pow(k, 0.64623f) + 
			0.9153f * std::pow(k, 0.99946f) - 0.90952f * k + 0.29523f;
	}
	else
	{
		rgb.y = 3985.0f * std::pow((k - 2600.8f), -1.0858f) + 0.45791f;
	}

	if (k <= 1900.0f)
	{
		rgb.z = 0.0f;
	}
	else if (k > 6600.0f)
	{
		rgb.z = 1.0f;
	}
	else
	{
		rgb.z = -6.107e-12f * std::pow(k, 3.0f) + 
			9.068E-8f * std::pow(k, 2.0f) - 1.9202e-4f * k + 7.4626e-2f;
	}

	return clamp(rgb);
}
}
} // end namespace wgt