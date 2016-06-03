import QtQuick 2.3
import QtQuick.Layouts 1.1


/*!
 \brief A vector4 variation of the vectorN control
*/

WGVectorN {
    id: vector4
    objectName: "WGVector4"

    property vector4d value

    vectorData: [value.x, value.y, value.z, value.w]
    vectorLabels: ["X:", "Y:", "Z:", "W:"]

    onElementChanged: {
        var temp_value = Qt.vector4d( vectorData[0], vectorData[1], vectorData[2], vectorData[3]);
        setValueHelper(vector4, "value", temp_value);
    }
}
