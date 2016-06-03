import QtQuick 2.3
import QtQuick.Layouts 1.1


/*!
 \brief A vectorRGBA variation of the vectorN control
*/

WGVector4 {
    objectName: "WGVectorRGBA"
    vectorLabels: ["R:", "G:", "B:", "A:"]
    minimumValues: [0, 0, 0, 0]
    maximumValues: [255, 255, 255, 255]
    decimals: [0, 0, 0, 0]
    stepSizes: [1, 1, 1, 1]
}

