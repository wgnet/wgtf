import QtQuick 2.5
import QtQuick.Layouts 1.3


/*!
 \ingroup wgcontrols
 \brief A vectorRGB variation of the vectorN control
*/

WGVector3 {
    objectName: "WGVectorRGB"
    WGComponent { type: "WGVectorRGB" }
    
    vectorLabels: ["R:", "G:", "B:"]
    minimumValue: 0
    maximumValue: 255
    decimals: 0
    stepsize: 1
}

