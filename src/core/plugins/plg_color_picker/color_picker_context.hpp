#ifndef COLOR_PICKER_CONTEXT_HPP
#define COLOR_PICKER_CONTEXT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector4.hpp"

#include <QPoint>

namespace wgt
{
class ColorPickerContext
{
	DECLARE_REFLECTED
public:
	void startObservingColor();
	void endObservingColor();
	Vector4 grabScreenColor( Vector2 p);

	Vector4 pixelColor_;
};
} // end namespace wgt
#endif // COLOR_PICKER_CONTEXT_HPP
