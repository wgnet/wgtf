import QtQuick 2.3
import QtQuick.Layouts 1.1


/*!
 \brief A vectorRGB variation of the vectorN control
*/

WGVector3 {
    objectName: "WGVectorRGB"
    vectorLabels: ["R:", "G:", "B:"]
    minimumValue: 0
    maximumValue: 255
    decimals: 0
    stepsize: 1
}

