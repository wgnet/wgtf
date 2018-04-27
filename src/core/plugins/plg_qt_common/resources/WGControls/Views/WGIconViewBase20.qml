import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Views 2.0

/** Internal WGIconView specific logic.
\ingroup wgcontrols */

GridView {
    id: gridViewBase
    WGComponent { type: "WGIconViewBase20" }

    property bool showItemText: true

    /** type:int Current row number.*/
    property alias currentRow: gridViewBase.currentIndex
    /** Link to the common view component.*/
    property var view
    /** Switch to load asynchronously or synchronously.*/
    property bool asynchronous: false
    /** This workaround is needed until the standard QML ScrollView is fixed. It is out by one pixel.*/
    readonly property var scrollViewError: view.clamp ? 0 : 1

    /*! This property determines the default size of the icons in the listview. A value < 32 will default contents to the list view */
    property int iconSize: 64

    /*! This property determines the size of the label of each icon */
    property int iconLabelSize: 9

    cellWidth: (iconSize + defaultSpacing.doubleBorderSize) < width ? width / Math.floor(width / (iconSize + defaultSpacing.doubleBorderSize)) : iconSize + defaultSpacing.doubleBorderSize
    cellHeight: (iconSize + defaultSpacing.doubleBorderSize) + iconLabelSize * 4

    /** The itemIndex of the current hovered item.*/
    property var hoveredIndex: null

    /** Signals that an item received a mouse press.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was hovered.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemHovered(var itemIndex, var rowIndex)

    snapMode: GridView.SnapToRow

    highlightFollowsCurrentItem: false

    property var getItemImageSource: function(itemData) {
        return "";
    }

    property var getItemText: function(itemData) {
        return "";
    }

    property var getItemToolTip: function(itemData) {
        return "";
    }


    property Component contentDelegate: ColumnLayout {
        id: itemContent
        objectName: typeof(iconLabel.text) != "undefined" ? iconLabel.text : "itemArea"
        spacing: 0
        anchors.fill: parent
        anchors.margins: defaultSpacing.standardBorderSize

        property var itemSelected: isSelected
        onItemSelectedChanged: {
            if (itemSelected) {
                if (iconLabel.truncated) {
                    iconLabel.font.pointSize = gridViewBase.iconLabelSize - 2
                }
            }
            else {
                iconLabel.font.pointSize = gridViewBase.iconLabelSize
            }
        }

        Item {
            Layout.preferredHeight: gridViewBase.iconSize
            Layout.preferredWidth: gridViewBase.iconSize
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

            WGImage {
                id: iconLoader
                height: gridViewBase.iconSize
                width: gridViewBase.iconSize
                fillMode: sourceSize.width > parent.width || sourceSize.height > parent.height ? Image.PreserveAspectFit : Image.Pad
                asynchronous: true
                source: gridViewBase.getItemImageSource(itemData)
            }
        }

        Text {
            id: iconLabel
            objectName: "IconLabel"

            text: gridViewBase.getItemText(itemData)
            horizontalAlignment: Text.AlignHCenter
            lineHeightMode: Text.FixedHeight
            lineHeight: gridViewBase.iconLabelSize + 2
            color: enabled ? palette.textColor : palette.disabledTextColor
            y: -2

            Layout.preferredWidth: parent.width - defaultSpacing.rowSpacing * 2
            Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

            maximumLineCount: {
                var lines = 2
                if (isSelected) {
                    lines += 2
                }
                if (gridViewBase.iconSize <= 32) {
                    lines += 1
                }
                return lines
            }
            wrapMode: Text.WrapAnywhere
            font.pointSize: gridViewBase.iconLabelSize
            font.bold: false
            elide: Text.ElideRight
        }
    }

    delegate: WGIconViewDelegate {
        id: iconDelegate
        view: gridViewBase.view
        Keys.forwardTo: [gridViewBase]

        WGToolTipBase {
            text: gridViewBase.getItemToolTip(model)
            tooltipArea: iconDelegate.itemMouseArea
        }

        itemDelegate: gridViewBase.contentDelegate

        width: gridViewBase.cellWidth
        height: gridViewBase.cellHeight

        // Pass events up to assetGrid.
        onItemPressed: gridViewBase.itemPressed(mouse, itemIndex, modelIndex);
        onItemClicked: gridViewBase.itemClicked(mouse, itemIndex, modelIndex)
        onItemDoubleClicked: gridViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
        onItemHovered: {
            gridViewBase.itemHovered(itemIndex, modelIndex)
            gridViewBase.hoveredIndex = itemIndex
        }
    }
}
