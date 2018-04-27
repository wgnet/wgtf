#include "pch.hpp"
#include "wg_types/color_utilities.hpp"
#include <vector>

namespace wgt
{
TEST(testToneMapping)
{
	std::vector<std::pair<float, std::vector<Vector4>>> toneMapSamples = 
    {
        std::make_pair<float, std::vector<Vector4>>(-4.0f,
        {
            ColorUtilities::makeColor(72, 255, 0),
            ColorUtilities::makeColor(15, 28, 0),
            ColorUtilities::makeColor(21, 38, 0),
            ColorUtilities::makeColor(29, 52, 0),
            ColorUtilities::makeColor(40, 72, 0),
            ColorUtilities::makeColor(56, 99, 0),
            ColorUtilities::makeColor(77, 136, 0),
            ColorUtilities::makeColor(106, 188, 0)
        }),
        std::make_pair<float, std::vector<Vector4>>(-1.0f,
        {
            ColorUtilities::makeColor(0, 59, 180),
            ColorUtilities::makeColor(0, 37, 61),
            ColorUtilities::makeColor(0, 51, 84),
            ColorUtilities::makeColor(0, 69, 116),
            ColorUtilities::makeColor(0, 96, 159),
            ColorUtilities::makeColor(0, 132, 219),
            ColorUtilities::makeColor(0, 182, 255),
            ColorUtilities::makeColor(0, 251, 255)
        }),
        std::make_pair<float, std::vector<Vector4>>(0.0f,
        {
            ColorUtilities::makeColor(69, 45, 8),
            ColorUtilities::makeColor(54, 45, 20),
            ColorUtilities::makeColor(75, 61, 28),
            ColorUtilities::makeColor(103, 84, 38),
            ColorUtilities::makeColor(141, 116, 53),
            ColorUtilities::makeColor(195, 160, 73),
            ColorUtilities::makeColor(255, 222, 101),
            ColorUtilities::makeColor(255, 255, 139)
        }),
        std::make_pair<float, std::vector<Vector4>>(0.0f,
        {
            ColorUtilities::makeColor(255, 255, 255),
            ColorUtilities::makeColor(99, 99, 99),
            ColorUtilities::makeColor(137, 137, 137),
            ColorUtilities::makeColor(189, 189, 189),
            ColorUtilities::makeColor(255, 255, 255),
            ColorUtilities::makeColor(255, 255, 255),
            ColorUtilities::makeColor(255, 255, 255),
            ColorUtilities::makeColor(255, 255, 255)
        }),
        std::make_pair<float, std::vector<Vector4>>(1.0f,
        {
            ColorUtilities::makeColor(69, 8, 8),
            ColorUtilities::makeColor(75, 28, 28),
            ColorUtilities::makeColor(102, 38, 38),
            ColorUtilities::makeColor(141, 53, 53),
            ColorUtilities::makeColor(194, 72, 72),
            ColorUtilities::makeColor(255, 100, 100),
            ColorUtilities::makeColor(255, 138, 138),
            ColorUtilities::makeColor(255, 191, 191)
        }),
        std::make_pair<float, std::vector<Vector4>>(4.0f,
        {
            ColorUtilities::makeColor(180, 0, 2),
            ColorUtilities::makeColor(255, 0, 39),
            ColorUtilities::makeColor(255, 0, 53),
            ColorUtilities::makeColor(255, 0, 74),
            ColorUtilities::makeColor(255, 0, 102),
            ColorUtilities::makeColor(255, 0, 141),
            ColorUtilities::makeColor(255, 0, 194),
            ColorUtilities::makeColor(255, 0, 255)
        })
    };

	for(const auto& sample : toneMapSamples)
    {
		const auto& color = sample.second.at(0);
		float offset = -3.0f;
        for(int i = 1; i <= 7; ++i)
        {
			const Vector4 hdr = ColorUtilities::setExposure(color, sample.first + offset);
			const Vector4 result = ColorUtilities::reinhardTonemap(hdr, 0.95f);
			const Vector4& expected = sample.second.at(i);
			CHECK(int(result.x * 255.0f) == int(expected.x * 255.0f));
            CHECK(int(result.y * 255.0f) == int(expected.y * 255.0f));
            CHECK(int(result.z * 255.0f) == int(expected.z * 255.0f));
            offset += 1.0f;
        }
    }
}
} // end namespace wgt
