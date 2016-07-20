import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0

/*!
 \brief Generates rows for both WGListView and WGTreeView.
*/
Item {
    id: itemRow
    objectName: "WGItemRow_" + index

    width: row.width != 0 ? row.width : 1024
    height: row.height != 0 ? row.height : 1024
    clip: true

	property var view
	property var depth: 0
	property var rowIndex: modelIndex
	property bool asynchronous: false

    /*! Pass parameters from mouse events up to parent.
        \see columnMouseArea for original event.
        \param mouse the MouseEvent that triggered the signal.
        \param itemIndex comes from C++.
               Call to modelIndex() automatically looks up the
               "C++ model index" from the row and column.
               Index of items inside the WGItemRow.
     */
    signal itemPressed(var mouse, var itemIndex)
    signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

    QtObject {
        id: internal
        property var implicitColumnWidths: []

        onImplicitColumnWidthsChanged: {
            var viewWidths = view.implicitColumnWidths;

            while (viewWidths.length < implicitColumnWidths.length)
            {
                viewWidths.push(0);
            }

            for (var i = 0; i < implicitColumnWidths.length; ++i)
            {
                viewWidths[i] = Math.max(viewWidths[i], implicitColumnWidths[i]);
            }

            view.implicitColumnWidths = viewWidths;
        }
    }

    Loader {
        id: rowBackground
        objectName: "RowBackground"
        anchors.fill: row

		asynchronous: itemRow.asynchronous
        property bool isSelected: view.selectionModel.isSelected(modelIndex)
        property bool isCurrent: view.selectionModel.currentIndex === modelIndex

        Connections {
            target: view.selectionModel
            onSelectionChanged: {
                rowBackground.isSelected = view.selectionModel.isSelected(modelIndex)
            }
        }

        sourceComponent: view.style.rowBackground
    }

    Component.onCompleted: {
        var implicitWidths = internal.implicitColumnWidths;

        while (implicitWidths.length < view.columnWidths.length) {
            implicitWidths.push(0);
        }

        internal.implicitColumnWidths = implicitWidths;
    }

    Row {
        id: row

        property var minX: rowHeader.width
        property var maxX: rowFooter.x - lastColumnHandleSpacer.width

        // Controls column spacing.
        Row {
            id: columnsLayout
            objectName: "Row"
            spacing: view.columnSpacing

            // Repeat columns horizontally.
            Repeater {
                id: columns
                objectName: "Columns"

                model: WGSequenceList {
                    id: rowModel
                    model: columnModel
                    sequence: view.columnSequence
                }

                delegate: Item {
                    id: columnContainer
                    objectName: "ColumnContainer_" + index
                    width: view.columnWidths[index]
                    height: childrenRect.height
                    clip: true

                    property bool isSelected: view.selectionModel.isSelected(modelIndex)
                    property bool isCurrent: view.selectionModel.currentIndex === modelIndex

                    Connections {
                        target: view.selectionModel
                        onSelectionChanged: {
                            columnBackground.isSelected = view.selectionModel.isSelected(modelIndex)
                        }
                    }

                    Loader {
                        id: columnBackground
                        objectName: "ColumnBackground"
                        anchors.fill: column
		                asynchronous: itemRow.asynchronous

                        property bool isSelected: columnContainer.isSelected
                        property bool isCurrent: columnContainer.isCurrent

                        sourceComponent: view.style.columnBackground
                    }

                    MouseArea {
                        id: columnMouseArea
                        objectName: "ColumnMouseArea"
                        width: view.columnWidths[index]
                        height: row.height
                        acceptedButtons: Qt.RightButton | Qt.LeftButton;

                        /*! Pass parameters from mouse events up to parent.
                            \param mouse the KeyEvent that triggered the signal.
                            \param itemIndex comes from C++.
                                   Call to modelIndex() automatically looks up the
                                   "C++ model index" from the row and column.
                                   Index of items inside the WGItemRow.
                         */
                        onPressed: itemPressed(mouse, modelIndex)
                        onClicked: itemClicked(mouse, modelIndex)
                        onDoubleClicked: itemDoubleClicked(mouse, modelIndex)
                    }

                    // Actual columns added after arrow
                    Item {
                        id: column
                        objectName: "Column"
                        x: Math.max(row.minX - columnContainer.x, 0)
                        width: Math.max(Math.min(row.maxX - columnContainer.x, columnContainer.width) - x, 0)
                        height: childrenRect.height
                        clip: true

                        property var minX: columnHeader.width
                        property var maxX: columnFooter.x

                        Item {
                            id: itemContainer
                            objectName: "ItemContainer"
                            x: column.minX
                            width: Math.max(column.maxX - x, 0)
                            height: childrenRect.height
                            clip: true

                            Loader {
                                id: itemDelegate
                                objectName: "ItemDelegate"
                                property var itemValue: model[view.columnRoles[index]]
                                property var itemData: model
                                property var itemDepth: depth
		                        asynchronous: itemRow.asynchronous
                                property int itemWidth: column.width
                                property bool isSelected: columnContainer.isSelected
                                property bool isCurrent: columnContainer.isCurrent
                                sourceComponent: view.columnDelegates[index]
                                onLoaded: itemContainer.updateImplicitWidths();
                            }

                            Connections {
                                target: itemDelegate.item
                                onImplicitWidthChanged: {
                                    if (itemDelegate.status === Loader.Ready) {
                                        itemContainer.updateImplicitWidths();
                                    }
                                }
                            }

                            function updateImplicitWidths()
                            {
                                var implicitWidths = internal.implicitColumnWidths;
                                implicitWidths[index] = itemDelegate.item.implicitWidth;

                                if (index === 0) {
                                    implicitWidths[index] += row.minX;
                                }

                                internal.implicitColumnWidths = implicitWidths;
                            }
                        }

                        Loader {
                            id: columnHeader
                            objectName: "ColumnHeader"
                            property var itemData: model
                            property var itemDepth: depth
		                    asynchronous: itemRow.asynchronous
                            height: column.height
                            width: sourceComponent.width
                            sourceComponent: view.style.columnHeader
                        }

                        Loader {
                            id: columnFooter
                            objectName: "ColumnFooter"
                            property var itemData: model
                            property var itemDepth: depth
                            asynchronous: itemRow.asynchronous
                            x: column.width - width
                            height: column.height
                            width: sourceComponent.width
                            sourceComponent: view.style.columnFooter
                        }
                    }
                }
            }
        }

        Item {
            id: lastColumnHandleSpacer
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: view.clamp ? 0 : columnSpacing
        }
    }

    Loader {
        id: rowHeader
        objectName: "RowHeader"
        property var itemData: model
        property var itemDepth: depth
		asynchronous: itemRow.asynchronous
        height: row.height
        width: sourceComponent.width
        sourceComponent: view.style.rowHeader
    }

    Loader {
        id: rowFooter
        objectName: "RowFooter"
        property var itemData: model
        property var itemDepth: depth
		asynchronous: itemRow.asynchronous
        x: row.width - width
        height: row.height
        width: sourceComponent.width
        sourceComponent: view.style.rowFooter
    }
}
