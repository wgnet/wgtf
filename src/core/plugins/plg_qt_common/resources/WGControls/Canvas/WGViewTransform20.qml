import QtQuick 2.5
import WGControls.Private 2.0

/*!
    \ingroup wgcontrols
*/
Item{
    WGComponent { type: "WGViewTransform20" }
    
	property var container;
    property real xScale: container ? container.width * .9 : 1;
    property real yScale: container ? -container.height * .8 : 1;
    property point origin: Qt.point(
        container ? container.width * .05 : 0,
        container ? container.height * .9 : 0)

    function transformX(val)
	{
		return val * xScale + origin.x;
	}

    function transformY(val)
	{
		return val * yScale + origin.y;
	}

    function transform(pointVal)
	{
        return Qt.point(transformX(pointVal.x), transformY(pointVal.y));
	}

    function inverseTransform(pointVal)
	{
		return Qt.point((pointVal.x - origin.x) / xScale, (pointVal.y - origin.y) / yScale);
	}

	function shift(delta)
	{
		origin.x += delta.x;
		origin.y += delta.y;
	}
}
