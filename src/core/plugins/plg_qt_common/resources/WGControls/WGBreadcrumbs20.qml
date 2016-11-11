import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0

/*!
 \ingroup wgcontrols
 \brief A control used to represent breadcrumb navigation of a tree view

Example:
\code{.js}
WGBreadcrumbs {
    model: folderModel
	modelIndex: selectedFolderIndex
}
\endcode
*/

Rectangle {
    id: rootFrame
    objectName: "WGBreadcrumbs"
    WGComponent { type: "WGBreadcrumbs20" }

    // Object properties
    Layout.fillHeight: false
    Layout.preferredHeight: defaultSpacing.minimumRowHeight
    Layout.fillWidth: true
    color: "transparent"

	property alias model: breadcrumbs.model
	property alias modelIndex: breadcrumbs.modelIndex
	property alias path: pathTextBox.text

	signal breadcrumbSelected(var modelIndex)
	signal pathEntered(var path)

	QtObject {
		id: internal

		property bool showBreadcrumbs: true
	}

    // Mouse area over the path text box
    MouseArea {
        objectName: "pathSelect"
        anchors.fill: parent
        enabled: internal.showBreadcrumbs
        hoverEnabled: true

        cursorShape: Qt.IBeamCursor

        onClicked: {
            internal.showBreadcrumbs = false
            pathTextBox.forceActiveFocus()
        }
    }

    // Text box to store the full, raw path of the breadcrumbs. May be used to manually navigate to
    // a specific path in the tree.
    WGTextBox {
        id: pathTextBox
        objectName: "pathTextBox"
        anchors.fill: parent
        visible: !internal.showBreadcrumbs

        onEditingFinished: {
            internal.showBreadcrumbs = true
        }

        onAccepted: {
			rootFrame.pathEntered(text);
        }
    }

    // Main layout of the breadcrumbs control.
    WGBreadcrumbsBase {
		id: breadcrumbs
		objectName: "breadcrumbs"
        visible: internal.showBreadcrumbs
		clip: true

		onBreadcrumbSelected: rootFrame.breadcrumbSelected(modelIndex)
    }
} // rootFrame
