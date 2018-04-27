import QtQuick 2.5
import QtQuick.Layouts 1.3
import WGControls.Private 2.0

/*!
 \ingroup wgcontrols
 \brief A vector2 variation of the vectorN control
*/

WGVectorN {
    objectName: "WGVector2"
    id: vector2
    WGComponent { type: "WGVector2" }

    property vector2d value

    vectorData: [value.x, value.y]
    vectorLabels: ["X:", "Y:"]

    onElementChanged: {
        var temp_value = Qt.vector2d( vectorData[0], vectorData[1]);
        setValueHelper(vector2, "value", temp_value);
    }
}
