#ifndef COLOR_UTILITIES_HPP
#define COLOR_UTILITIES_HPP

#include "vector4.hpp"
#include <array>

namespace wgt
{
namespace ColorComponent
{
	enum
	{
		Red,
		Green,
		Blue,
		Alpha
	};
}

namespace ColorUtilities
{
typedef std::array<int, 4> Int4;

/**
* Creates a color between 0->1 from RGBA components between 0->255
*/
Vector4 makeColor(int r, int g, int b, int a = 255);

/**
* Creates a color between 0->255 from RGBA components between 0->1
*/
Int4 makeColor(const Vector4& color);

/**
* Creates a HDR color from RGB components between 0->255 and an exposure intensity
*/
Vector4 makeHDRColor(int r, int g, int b, float intensity);

/**
* Sets the exposure on a HDR color
*/
Vector4 setExposure(const Vector4& v, float intensity);

/**
* Ensures each color component is between 0 and 1
*/
Vector4 clamp(const Vector4& v);

/**
* Applies Reinhard tone mapping operator
* Reference: http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
* @param v RGB HDR color
* @return RGB LDR color
*/
Vector4 reinhardTonemap(const Vector4& v, float luminanceWhite);

/**
* @return the luminance of the RGB color
*/
float luminance(const Vector4& v);

/**
* Converts Kelvin values to RGB
* @note based on Titan Editor's ConvertKelvinToRGB
*/
Vector4 convertKelvinToRGB(int kelvin);

}
} // end namespace wgt
#endif // COLOR_UTILITIES_HPP