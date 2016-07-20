import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

//TODO This needs to be tested with a tree with multiple root nodes

/*!
    \brief Items that make the rows in a WGTreeView.
    Should only be used within a WGTreeView. Not intended to be used on its own.
    The WGTreeItem is a ListView that loads further treeItems via a delegate loader.

Example:
\code{.js}
    WGTreeItem {
        id: rootItem
        model: treeView.model
        width: treeView.width
        height: treeView.height
    }
\endcode
*/

ListView {
    id: treeItem
    objectName: typeof(itemData) != "undefined" ? itemData.indexPath : "WGListView"

    property real columnSpacing: treeView.columnSpacing
    property real selectionMargin: treeView.selectionMargin
    property real minimumRowHeight: treeView.minimumRowHeight

    property var selectionExtension: treeView.selectionExtension
    property var treeExtension: treeView.treeExtension

    property int columnCount: treeView.columnCount
    property var columnWidths: treeView.columnWidths
    property var columnDelegates: treeView.columnDelegates
    property var defaultColumnDelegate: treeView.defaultColumnDelegate

    property int depth: typeof(childItems) === "undefined" ? 0 : childItems.depth
    property int parentListIndex: typeof(index) === "undefined" ? 0 : index
    property real marginedWidth: width - leftMargin - rightMargin - minimumScrollbarWidth

    // Local properties and methods for handling multiselection during keyboard navigation
    property bool modifiedSelectionExtension: false;
    property bool shiftKeyPressed: false

    function handlePreNavigation() {
        if (selectionExtension == null) {
            return
        }
        if (selectionExtension.multiSelect && !shiftKeyPressed) {
            selectionExtension.multiSelect = false;
            modifiedSelectionExtension = true;
        }
    }

    function handlePostNavigation() {
        if (selectionExtension == null) {
            return
        }
        if (modifiedSelectionExtension == true) {
            selectionExtension.multiSelect = true;
        }
    }

    function setCurrentIndex( modelIndexToSet ) {
        if (treeExtension !== null)
        {
            treeExtension.currentIndex = modelIndexToSet
        }
        // Give the parent active focus, so it can handle keyboard inputs
        if (typeof content !== "undefined")
        {
            content.forceActiveFocus()
        }
        else
        {
            forceActiveFocus()
        }
    }

    Component.onCompleted: {
        treeView.addDepthLevel(depth);
    }

    Component.onDestruction: {
        treeView.removeDepthLevel(depth);
    }

    model: childModel
    height: visible ? contentHeight + topMargin + bottomMargin : 0
    leftMargin: 0
    rightMargin: 0
    topMargin: treeView.childListMargin
    bottomMargin: treeView.childListMargin

    //The rectangle for the entire row
    delegate: Item {
        id: itemDelegate

        property real actualIndentation: treeView.indentation * depth
        property real verticalMargins: !hasChildren ? childRowMargin * 2 : expanded ? 0 : headerRowMargin
        readonly property bool oddDepth: depth % 2 !== 0
        readonly property bool oddIndex: treeItem.parentListIndex % 2 !== 0
        readonly property bool switchRowColours: oddDepth !== oddIndex

        height: content.height + treeView.footerSpacing + verticalMargins
        width: treeItem.marginedWidth

        Rectangle {
            id: groupBackgroundColour
            x: actualIndentation
            width: treeItem.marginedWidth - x
            height: parent.height
            visible: treeView.backgroundColourMode === treeView.incrementalGroupBackgroundColours
            color: visible ? Qt.lighter(treeView.backgroundColour, 1 + depth % treeView.backgroundColourIncrements / 10) : "transparent"
        }

        Rectangle { // separator line between rows
            id: topSeparator
            width: parent.width
            anchors.top: parent.top
            anchors.horizontalCenterOffset: -(content.height + treeView.footerSpacing)
            height: 1
            color: Qt.darker(palette.midLightColor,1.2)
            visible: treeView.lineSeparator && depth !== 0
        }

        Item { // All content
            id: content
            objectName: "content"
            height: childrenRect.height
            y: hasChildren ? treeView.headerRowMargin : treeView.childRowMargin
            anchors.left: parent.left
            anchors.right: parent.right

            property bool hasActiveFocus: false

            Component.onCompleted: {
                if(treeItem.depth === 0)
                {
                    if (treeView.rootExpanded && hasChildren)
                    {
                        expanded = true;
                    }
                }
            }

            Keys.onUpPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveUp();
                treeItem.handlePostNavigation();
            }

            Keys.onDownPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveDown();
                treeItem.handlePostNavigation();
            }

            Keys.onLeftPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveLeft();
                treeItem.handlePostNavigation();
            }

            Keys.onRightPressed: {
                treeExtension.blockSelection = true;

                treeItem.handlePreNavigation();
                treeExtension.moveRight();
                treeItem.handlePostNavigation();
            }

            Keys.onReturnPressed: {
                if (treeExtension.blockSelection) {
                    return;
                }

                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            Keys.onSpacePressed: {
                if (treeExtension.blockSelection) {
                    return;
                }

                // Select the current item in tree
                treeExtension.blockSelection = false;
                treeExtension.selectItem();
            }

            Keys.onPressed: {
                // Flag the shift key being pressed to allow multiselection via tree navigation
                if (event.key == Qt.Key_Shift) {
                    shiftKeyPressed = true;
                }
                event.accepted = false;
            }

            Keys.onReleased: {
                // Flag the shift key being released to disallow multiselection via tree navigation
                if (event.key == Qt.Key_Shift) {
                    shiftKeyPressed = false;
                }
                event.accepted = false;
            }

            onActiveFocusChanged: {
                if (content.activeFocus)
                {
                    hasActiveFocus = true
                }
                else
                {
                    hasActiveFocus = false
                }
            }

            WGListViewRowDelegate { // The row
                objectName: typeof(display) != "undefined" ? "WGListViewRowDelegate_" + display : "WGListViewRowDelegate"

                id: rowDelegate

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

				columnSequence: treeView.columnSequence
                columnWidths: treeView.columnWidths
                columnSpacing: treeView.columnSpacing
                defaultColumnDelegate: headerColumnDelegate
                hasActiveFocusDelegate: content.hasActiveFocus
                indentation: treeView.indentation * depth
                showBackgroundColour:
                    treeView.backgroundColourMode === treeView.uniformRowBackgroundColours ||
                    treeView.backgroundColourMode === treeView.alternatingRowBackgroundColours
                backgroundColour: itemDelegate.switchRowColours ? treeView.alternateBackgroundColour : treeView.backgroundColour
                alternateBackgroundColour: itemDelegate.switchRowColours ? treeView.backgroundColour : treeView.alternateBackgroundColour

                columnDelegates: []
                selectionExtension: treeView.selectionExtension
                modelIndex: treeView.model.index(rowIndex, 0, parentIndex)

                onClicked: {
                    if (treeExtension && treeExtension.blockSelection) {
                        return;
                    }

                    var modelIndex = treeView.model.index(rowIndex, 0, parentIndex);
                    treeView.rowClicked(mouse, modelIndex);
                    currentIndex = rowIndex;

                    // Update the treeExtension's currentIndex
                    if (treeExtension !== null)
                    {
                        treeExtension.currentIndex = modelIndex;
                    }

                    // Give the parent active focus, so it can handle keyboard inputs
                    content.forceActiveFocus()
                }

                onDoubleClicked: {
                    if (treeExtension && treeExtension.blockSelection) {
                        return;
                    }

                    var modelIndex = treeView.model.index(rowIndex, 0, parentIndex);
                    treeView.rowDoubleClicked(mouse, modelIndex);
                    toggleExpandRow();
                    currentIndex = rowIndex;

                    // Update the treeExtension's currentIndex
                    setCurrentIndex( modelIndex )
                }

                function isExpandable()
                {
                    return (hasChildren && typeof expanded !== "undefined");
                }


                function toggleExpandRow()
                {
                    if (isExpandable())
                    {
                        expanded = !expanded;
                    }
                }

                // return - true - if child tree is expanded
                function expandRow()
                {
                    if (isExpandable() && !expanded)
                    {
                        expanded = true;

                        // handled
                        return true;
                    }

                    // No children, non expandable, or already expanded
                    return false;
                }

                // return - true - if child tree is collapsed
                function collapseRow()
                {
                    if (isExpandable() && expanded)
                    {
                        expanded = false;

                        // handled
                        return true;
                    }

                    // No children, non expandable, or collapsed
                    return false;
                }

                Component { // Row contents, icons and columns
                    id: headerColumnDelegate

                    Item {
                        id: header
                        height: headerContent.status === Loader.Ready ? headerContent.height : expandIconArea.height
                        property var parentItemData: itemData
                        property bool firstColumn: columnIndex === 0
                        property bool showExpandIcon: firstColumn && hasChildren

                        Rectangle {
                            id: expandIconArea
                            objectName: typeof(parentItemData) != "undefined" ? "expandIconArea_" + parentItemData.display : "expandIconArea"
                            color: "transparent"
                            width: firstColumn ? expandButton.x + expandButton.width + expandIconMargin : 0
                            height: Math.max(minimumRowHeight, treeView.expandIconSize)

                            onWidthChanged: treeView.setExpandIconWidth(width)

                            Text {
                                id: expandButton
                                objectName: typeof(parentItemData) != "undefined" ? "expandButton_" + parentItemData.display : "expandButton"
                                color:
                                    !showExpandIcon ? "transparent" :
                                    expandMouseArea.containsMouse ? palette.highlightColor :
                                    expanded ? palette.textColor :
                                    palette.neutralTextColor

                                width: firstColumn ? paintedWidth : 0
                                font.family : "Marlett"
                                font.pixelSize: treeView.expandIconSize
                                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
                                text : expanded ? "\uF036" : "\uF034"
                                x: expandIconMargin
                                anchors.verticalCenter: parent.verticalCenter
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }

                            MouseArea {
                                id: expandMouseArea
                                objectName: "expandMouseArea"
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: showExpandIcon ? expandButton.x + expandButton.width + expandIconMargin : 0
                                hoverEnabled: true
                                enabled: showExpandIcon

                                onPressed: {
                                    rowDelegate.toggleExpandRow()
                                }
                            }
                        }

                        Loader {
                            id: headerContent
                            anchors.top: parent.top
                            anchors.left: expandIconArea.right
                            anchors.right: header.right
                            property var itemData: parentItemData

                            sourceComponent: // if a column delegate is defined use it, otherwise use default
                                columnIndex < treeView.columnDelegates.length ? treeView.columnDelegates[columnIndex]
                                : treeView.defaultColumnDelegate

                            onLoaded: {
                                height = Math.max(expandIconArea.height, item.height);
                                rowDelegate.height = height;
                            }
                        }
                    }
                }

                property var isSelected: typeof selected != 'undefined' ? selected : false
                onIsSelectedChanged: {
                    if (!isSelected) {
                        return;
                    }

                    var listView = treeItem;
                    while (listView != null &&
                    (typeof listView.enableVerticalScrollBar == 'undefined' || listView.enableVerticalScrollBar == false)) {
                        listView = listView.parent;
                    }
                    if (listView == null) {
                        return;
                    }

                    var scrollBar = listView.verticalScrollBar.scrollFlickable;
                    var scrollHeight = Math.floor(scrollBar.contentHeight * scrollBar.visibleArea.heightRatio);

                    var item = rowDelegate;
                    var itemY = scrollBar.contentY;
                    var itemHeight = item.height;
                    while (item != null && item != listView) {
                        itemY += item.y;
                        item = item.parent;
                    }
                    if (item == null) {
                        return;
                    }

                    if (itemY < scrollBar.contentY) {
                        scrollBar.contentY = itemY;
                    }
                    else if (itemY + itemHeight > scrollBar.contentY + scrollHeight) {
                        scrollBar.contentY = itemY + itemHeight - scrollHeight;
                    }
                }
            }

            Item {
                id: childItems
                anchors.right: parent.right
                anchors.left: parent.left
                y: rowDelegate.y + rowDelegate.height + (hasChildren ? treeView.headerRowMargin : 0) + (expanded ? childListMargin : 0)
                height: visible ? subTree.height : 0
                visible: !ancestorCollapsed

                property int depth: treeItem.depth + 1
                property bool ancestorCollapsed: !treeItem.visible || typeof expanded === "undefined" || !expanded || subTree.status !== Loader.Ready

                Loader {
                    id: subTree
                    source: "WGTreeItem.qml"
                    width: treeItem.marginedWidth
                }
            }
        }
    }
}
