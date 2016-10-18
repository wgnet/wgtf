import QtQuick 2.5
import QtQuick.Layouts 1.3


/*!
 \ingroup wgcontrols
 \brief A vectorRGBA variation of the vectorN control
*/

WGVector4 {
    objectName: "WGVectorRGBA"
    WGComponent { type: "WGVectorRGBA" }
    
    vectorLabels: ["R:", "G:", "B:", "A:"]
    minimumValues: [0, 0, 0, 0]
    maximumValues: [255, 255, 255, 255]
    decimals: [0, 0, 0, 0]
    stepSizes: [1, 1, 1, 1]
}

