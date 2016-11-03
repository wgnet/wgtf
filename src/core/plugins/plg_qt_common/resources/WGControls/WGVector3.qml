import QtQuick 2.5
import QtQuick.Layouts 1.3
import WGControls.Private 1.0


/*!
 \ingroup wgcontrols
 \brief A vector3 variation of the vectorN control
*/

WGVectorN {
    id: vector3
    objectName: "WGVector3"
    WGComponent { type: "WGVector3" }

    property vector3d value

    vectorData: [value.x, value.y, value.z]
    vectorLabels: ["X:", "Y:", "Z:"]

    onElementChanged: {
        var temp_value = Qt.vector3d( vectorData[0], vectorData[1], vectorData[2]);
        setValueHelper(vector3, "value", temp_value);
    }
}
