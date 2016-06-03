import QtQuick 2.3

//TODO: I cant find this being used anywhere. Is it still needed?

/*!
    \brief Use within controls like WGColumnLayout that do not allow binding to top and bottom
    Recommend NOT using scrollpanels within scrollpanels where possible.
*/

WGScrollPanel {
    objectName: "WGColumnScrollPanel"
    anchors.bottom: undefined
    anchors.top: undefined
}
