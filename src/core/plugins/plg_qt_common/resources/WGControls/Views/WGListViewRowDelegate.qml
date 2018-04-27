import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 1.0


/*!
 \ingroup wgcontrols
 \brief WGListViewRowDelegate is used within WGListView's delegate.
 WGListViewRowDelegate will load custom column delegates in its delegate or fall back to a default if none exists.
 WGListViewRowDelegate should only be used within the contexts of a ListView.
 See WGTreeItem for an example of its use.
*/

Item {
    id: rowDelegate
    objectName: typeof(model.display) != "undefined" ? "WGListViewRowDelegate_" + model.display : "WGListViewRowDelegate"
    WGComponent { type: "WGListViewRowDelegate" }

    /*!
        This property defines the indentation before the first element on each row
        The default value is \c 0
    */
    property int indentation: 0

    /*!
        This property holds the index of the selected row in the list
    */
    property int rowIndex: index

    /*!
        This property represents the model index (QModelIndex) of the selected row in the list
    */
    property var modelIndex: null

    /*!
        This property contains a default column delegate.
        The default value is \c null
    */
    property var defaultColumnDelegate: null

    /*!
        This property contains the items to be delegated by the WGListViewRowDelegate's delegate.
        The default value is an empty list
    */
    property var columnDelegates: []

    property var columnSequence: []

    /*! This property contains the column widths */
    property var columnWidths: []

    property real columnSpacing: 0

    /*!
        This property describes mouse selection behaviour
    */
    property var selectionExtension: null

    /*! This property holds the active focus state of the control
        The default value is \c false */
    property bool hasActiveFocusDelegate: false

    /*! This property specifies the main colour for the row background */
    property bool showBackgroundColour: false

    /*! This property specifies the main colour for the row background */
    property color backgroundColour: palette.midDarkColor

    /*! This property specifies the alternate colour for the row background */
    property color alternateBackgroundColour: Qt.darker(palette.midLightColor,1.2)

	property bool asynchronous: false

	property alias dragItem: dragItem

    /*! This signal is sent on a single click
    */
    signal clicked(var mouse)

    //TODO: Improve documentation
    /*! This signal is sent on a double click
    */
    signal doubleClicked(var mouse)

	signal rowReady();

    function calculateMaxTextWidth(index)
    {
        var maxTextWidth = 0;
        var parentComponent = itemMouseArea.columnsList;
        var childObject = parentComponent.children[index]

        if (!childObject.visible)
        {
            return maxTextWidth;
        }

        // if it has a painted width, turn off elide,
        // check if its painted width is the longest then update and reset elide
        var childElide = Text.ElideNone

        if (childObject.elide != Text.ElideNone)
        {
            childElide = childObject.elide
            childObject.elide = Text.ElideNone
        }

        maxTextWidth = childObject.paintedWidth

        if(childElide != childObject.elide)
        {
            childObject.elide = childElide
        }

        return maxTextWidth;
    }

    height: minimumRowHeight
    clip: true

    Item {
        id: dragItem
        parent: rowDelegate.parent
        anchors.fill: rowDelegate
        Drag.active: itemMouseArea.drag.active;
        Drag.hotSpot.x: width * 0.5
        Drag.hotSpot.y: height * 0.5
        Drag.source: rowDelegate
        Drag.dragType: Drag.Automatic 
        Drag.proposedAction: Qt.CopyAction
        Drag.supportedActions: Qt.CopyAction

        Drag.onDragFinished: {
            makeFakeMouseRelease();
            x = parent.x;
            y = parent.y;
        }

        MouseArea {
            id: itemMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.RightButton | Qt.LeftButton;
            drag.target: dragItem

            property var columnsList: columns;

            onPressed: {
                if ((selectionExtension == null) || (typeof selected == 'undefined'))
                {
                    return;
                }

                if (mouse.button == Qt.LeftButton || mouse.button == Qt.RightButton)
                {
                    var multiSelect = selectionExtension.multiSelect;

                    if (mouse.modifiers & Qt.ControlModifier)
                    {
                        selected = !selected;
                    }
                    else if (mouse.modifiers & Qt.ShiftModifier)
                    {
                        if (multiSelect)
                        {
                            selectionExtension.prepareRangeSelect();
                            selected = true;
                        }
                    }
                    else
                    {
                        if (multiSelect && ((mouse.button == Qt.LeftButton) || (mouse.button == Qt.RightButton && modelIndex != null && !selectionExtension.indexInSelection(modelIndex))))
                        {
                            selectionExtension.clearOnNextSelect();
                        }

                        selected = true;
                    }
                }
            }

            onClicked: {
                rowDelegate.clicked(mouse)
                // NOTE: Do not give the parent active focus here. The tree view and the list view have different ways to utilize
                //		 us, so giving parent focus will break keyboard input event handles.
            }

            onDoubleClicked: rowDelegate.doubleClicked(mouse)

            Rectangle {
                id: background
                anchors.fill: parent
                visible: rowDelegate.showBackgroundColour
                color: index % 2 === 0 ? rowDelegate.backgroundColour : rowDelegate.alternateBackgroundColour
            }

            Rectangle {
                id: selectionHighlight
                color: hasActiveFocusDelegate ? palette.highlightShade : "grey"
                anchors.fill: itemMouseArea
                anchors.margins: selectionMargin
                visible: !itemMouseArea.pressed && typeof selected != 'undefined' && selected
            }

            Rectangle {
                id: mouseOverHighlight
                anchors.fill: itemMouseArea
                visible: itemMouseArea.containsMouse
                opacity: 0.5
                color: palette.highlightShade
            }

            ListView {
                id: columns

                // Adapt from number of columns in the model to the number of
                // columns in the view.
                // @see WGListView.columnSequence
                model: WGColumnLayoutProxy {
                    sourceModel: columnModel
                    columnSequence: rowDelegate.columnSequence.length === 0 ? [0] : rowDelegate.columnSequence;
                }

                x: indentation
                width: Math.max(0, parent.width - indentation)
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                orientation: Qt.Horizontal
                interactive: false
                spacing: rowDelegate.columnSpacing
                property int loadedItemCount: 0

                delegate: Loader {
                    id: columnDelegate

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    property var itemData: model
                    property int rowIndex: rowDelegate.rowIndex
                    property int columnIndex: index
                    property int indentation: rowDelegate.indentation
                    asynchronous: rowDelegate.asynchronous

                    sourceComponent:
                    columnIndex < columnDelegates.length ? columnDelegates[columnIndex] :
                    defaultColumnDelegate

                    onLoaded: {
                        columns.loadedItemCount++;
                        if(columns.loadedItemCount == columns.count)
                        {
                            rowDelegate.rowReady();
                        }

                        var widthFunction = function()
                        {
                            var columnWidths = rowDelegate.columnWidths;
                            var columnWidth = 0;

                            if (columnWidths.length === 0)
                            {
                                columnWidth = Math.ceil(rowDelegate.width / columns.count - rowDelegate.columnSpacing);
                            }
                            else if (columnIndex < columnWidths.length)
                            {
                                columnWidth = columnWidths[columnIndex];
                            }

                            if (columnIndex === 0)
                            {
                                columnWidth -= indentation;
                            }

                            return Math.max(0, columnWidth);
                        }

                        item.width = Qt.binding(widthFunction);
                        rowDelegate.height = Math.max(height, minimumRowHeight);
                        item.clip = true;
                    }
                }
            }
        }
    }
}
