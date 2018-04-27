import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 2.0
import WGControls.Layouts 2.0
import WGControls.Global 2.0

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
    clip: true

    border.color: pathMouseover.containsMouse ? palette.lightShade : "transparent"
    border.width: defaultSpacing.standardBorderSize

    property var fontSize: breadcrumbs.defaultFontSize
    property alias model: breadcrumbs.model
    property alias modelIndex: breadcrumbs.modelIndex

	onModelIndexChanged: {
		pathTextBox.text = modelIndex != null ? getPath(modelIndex) : ""
	}

    signal breadcrumbSelected(var modelIndex)

	function getPath(modelIndex) {
		var parentIndex = model.parent(modelIndex)
		if (parentIndex == modelIndex) {
			return "";
		}

		var path = model.data(modelIndex);
		if (path == undefined) {
			return "";
		}

		var parentPath = getPath(parentIndex);
		return parentPath != "" ? parentPath + "\\" + path : path;
	}

	function getIndex(path, parentIndex) {
		var rowCount = parentIndex != null ? model.rowCount(parentIndex) : model.rowCount();
        for (var row = 0; row < rowCount; ++row) {
            var rowIndex = parentIndex != null ? model.index(row, 0, parentIndex) : model.index(row, 0);
            var rowPath = model.data(rowIndex);

            var pathN = WGPath.normalisePath(path);
            var rowPathN = WGPath.normalisePath(rowPath);
            if (pathN == rowPathN) {
                return rowIndex;
            }

			rowPathN += "\\";
			if (pathN.indexOf(rowPathN) == 0) {
				var childPath = pathN.substring(rowPathN.length);
                var childIndex = getIndex(childPath, rowIndex);
                if (childIndex != null) {
                    return childIndex;
                }
            }
		}
		return null;
	}

	function selectPath(path) {
		var index = getIndex(path);
		if (index == null) {
			pathTextBox.text = getPath(modelIndex);
		}
		else {
			modelIndex = index;
			breadcrumbSelected(modelIndex);
		}
	}

    QtObject {
        id: internal

        property bool showBreadcrumbs: true
    }

    // Mouse area over the path text box
    MouseArea {
        id: pathMouseover
        objectName: "pathSelect"
        anchors.fill: parent
        enabled: internal.showBreadcrumbs
        hoverEnabled: true

        cursorShape: Qt.IBeamCursor

        onClicked: {
			pathTextBox.text = getPath(modelIndex)
			pathTextBox.selectAll()
			pathTextBox.forceActiveFocus()
			internal.showBreadcrumbs = false
        }
    }

    // Text box to store the full, raw path of the breadcrumbs. May be used to manually navigate to
    // a specific path in the tree.
    WGTextBox {
        id: pathTextBox
        objectName: "pathTextBox"
        anchors.fill: parent
        visible: !internal.showBreadcrumbs

		onActiveFocusChanged: {
			internal.showBreadcrumbs = !activeFocus
		}

        onEditingFinished: {
			internal.showBreadcrumbs = true;
            rootFrame.selectPath(text);
        }

        onAccepted: {
			internal.showBreadcrumbs = true;
            rootFrame.selectPath(text);
        }
    }

    // Main layout of the breadcrumbs control.
    WGBreadcrumbsBase {
        id: breadcrumbs
        objectName: "breadcrumbs"
        visible: internal.showBreadcrumbs
        clip: true
        fontSize: rootFrame.fontSize

        x: width > parent.width ? parent.width - width : 0

        onBreadcrumbSelected: rootFrame.breadcrumbSelected(modelIndex)
    }
} // rootFrame
