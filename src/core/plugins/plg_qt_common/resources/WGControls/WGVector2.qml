import QtQuick 2.3
import QtQuick.Layouts 1.1
import WGControls 1.0

/*!
 \brief A vector2 variation of the vectorN control
*/

WGVectorN {
    objectName: "WGVector2"
    id: vector2
    property vector2d value

    vectorData: [value.x, value.y]
    vectorLabels: ["X:", "Y:"]

    onElementChanged: {
        var temp_value = Qt.vector2d( vectorData[0], vectorData[1]);
        setValueHelper(vector2, "value", temp_value);
    }
}
