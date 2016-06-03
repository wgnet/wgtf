import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1


ScrollView {
    objectName:  itemData != null ? itemData.indexPath : "TreeView"
    property variant model_ : source

    property int columnCount_ : 1
    property int columnWidth_ : 200
    property variant columnWidths_ : []
    property variant columnSequence_ : []

    readonly property color colorText_ : palette.textColor   //Heading text, body text
    property variant colorHeader_ : [palette.darkerShade,palette.darkestShade,palette.darkerShade]   //by depth_
    property variant colorBody_ : [palette.darkerShade,palette.darkShade,palette.darkerShade] //by depth_
    property color colorHighlight_ : palette.highlightColor  //indentLine
    property color colorLighten_ : palette.lighterShade //makes slightly lighter, splitterHandle
    property color colorDarken_ : palette.darkerShade   //makes slightly darker, splitterHandle

    property Component columnDelegate_ : RowLayout {
        Image{
            source: itemData_.thumbnail
        }
        Text{
            color : colorText_
            clip : itemData_.component == null ? false : true
            text : itemData_ != null ? itemData_.display : ""
            font.bold : itemData_.hasChildren
            verticalAlignment: Text.AlignVCenter
        }
        Text{}
    }

    property variant columnDelegates_ : []
    property int itemHeight_ : 18
    property bool clampWidth_ : false

    property int indentation_ : 5
    property variant showIndentLine_ : [false,true,false] //by depth_

    property int borderWidth_ : 5
    property int itemMargin_ : 3

    property int splitterWidth_ : 2
    property int splitterMouseWidth_ : 5
    property bool splitterShadow_ : true //best with splitterWidth_ : 2
    property var currentItem : root.currentItem

    id : base
    anchors.fill : parent

    Item {
        width : columns.x + columns.width
        height : root.height

        TreeItem {
            id : root
            objectName: "root"
            width : parent.width
            model_ : base.model_
        }

        Item {
            id : columns
            x : splitterWidth_
            y : borderWidth_
            width : childrenRect.width
            height : childrenRect.height

            Repeater {
                model : columnCount_
                delegate : Rectangle {
                    id : column
                    objejctName: "column"
                    width : handle.x + handle.width
                    height : root.height - borderWidth_ * 2
                    x : index == 0 ? 0 : columns.children[index - 1].x + columns.children[index - 1].width
                    y : root.y
                    color : "transparent"

                    Rectangle {
                        id : handle
                        x : index < columnWidths_.length ? columnWidths_[index] : columnWidth_
                        width : splitterWidth_
                        height : parent.height
                        color : "transparent"

                        MouseArea {
                            x : -Math.min(handle.x, splitterMouseWidth_)
                            y : -borderWidth_
                            width : parent.width - x + splitterMouseWidth_ + splitterWidth_
                            height : parent.height + borderWidth_ * 2
                            drag.target : parent
                            drag.axis : Drag.XAxis
                            drag.minimumX : 0
                            drag.maximumX : clampWidth_ ? base.viewport.width - borderWidth_ * 2 - splitterWidth_ - column.x : Number.MAX_VALUE
                            enabled : !clampWidth_ || index < columnCount_ - 1
                        }

                        onXChanged : {
                            updateColumns();
                        }
                    }
                }
            }
        }
    }

    property bool initialised_ : false
    property bool updatingColumns_ : false

    Component.onCompleted : {
        initialised_ = true;
    }

    viewport.onWidthChanged : {
        updateColumns();
    }

    function columnWidth( obj, index ) {
        var column = columns.children[index];
        var width = column != null ? column.width : 0;
        if (index == 0)
        {
            while (obj != root && obj != null)
            {
                width -= obj.x;
                obj = obj.parent;
            }
            width += splitterWidth_;
        }
        return width;
    }

    function updateColumns() {
        if (!clampWidth_) {
            return;
        }

        if (!initialised_) {
            return;
        }

        if (updatingColumns_) {
            return;
        }

        updatingColumns_ = true;
        var x = 0;
        for (var i = 0; i < columnCount_; ++i) {
            var column = columns.children[i];
            var handle = column.children[0];
            var maxWidth = base.viewport.width - borderWidth_ * 2 - x;
            var maxX = maxWidth - splitterWidth_;
            if (i < columnCount_ - 1) {
                handle.x = Math.min(handle.x, maxX);
            }
            else {
                handle.x = maxX;
            }
            x += handle.x + splitterWidth_;
        }
        updatingColumns_ = false;
    }
}
