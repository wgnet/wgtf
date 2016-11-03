import QtQuick 2.5

/*!
    \ingroup wgcontrols
    \brief Use within controls like WGColumnLayout that do not allow binding to top and bottom
    Recommend NOT using scrollpanels within scrollpanels where possible.
	\todo I cant find this being used anywhere. Is it still needed?
*/

WGScrollPanel {
    objectName: "WGColumnScrollPanel"
    WGComponent { type: "WGColumnScrollPanel" }
    anchors.bottom: undefined
    anchors.top: undefined
}
