import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import WGControls 1.0

ListView {
    objectName:  itemData != null ? itemData.indexPath : "TreeItem"
    WGComponent { type: "TreeItem" }
    
    property bool expanded_ : typeof expanded != "undefined" ? expanded : false
    property variant model_ : childModel
    property int depth_ : 0
    signal doubleClicked( var clickedItem )

    id : view
    boundsBehavior : Flickable.StopAtBounds
    anchors.top : parent.top
    anchors.topMargin : {
        var topMargin = 0;
        if (expanded_) {
            topMargin += borderWidth_;	//Bottom of the header
            topMargin += itemMargin_;	//Between the header and body
        }
        return topMargin;
    }
    height : {
        var height = y + childrenRect.height;
        if (expanded_) {
            height += itemMargin_; //Between the body and the footer
        }
        return height
    }
    spacing : itemMargin_
    model : model_



    delegate : Rectangle {
        property bool hasChildren_ : hasChildren
        property bool expanded_ : expanded
        property bool selected_ : selected


        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( value )
            }

            onDataPasted : {
                value = data
            }

        }

        id : header
        color : {
            var color = "transparent";
            if (hasChildren_) {
                if (depth_ < colorHeader_.length) {
                    color = colorHeader_[depth_]
                } else {
                    color = colorHeader_[colorHeader_.length - 1]
                }
            }
            return color
        }
        radius : hasChildren_ ? borderWidth_ : 0
        width : parent.width
        height : content.height + radius * 2

        Rectangle {
            id : headerMask // Mask behind the header rows that hides the exposed bits of the splitter handle
            color : {
                var color = "transparent";
                if (hasChildren_ && depth_ > 0) {
                    var depth = depth_ - 1; //needs to use the color of the depth above it.
                    if (depth < colorBody_.length) {
                        color = colorBody_[depth]
                    } else {
                        color = colorBody_[colorBody_.length - 1]
                    }
                }
                return color
            }

            width : body.width
            height: parent.height + itemMargin_ * 2
            y : -itemMargin_
            z : -1
        }

        Rectangle {
            id : headerEx
            color : header.color
            height : header.height
            x : columnWidth( header, 0 )
            width : header.width - x - borderWidth_
        }

        Rectangle {
            id: expandMouseArea
            objectName: "expandItem"

            property bool entered_ : false
            property real columnStart : hasChildren_ ? 16 : 0 //TODO: Change to icon width, perhaps in the new WGTreeView

            anchors.top: header.top
            anchors.left: header.left
            anchors.right: hasChildren_ ? header.right : headerEx.left //extend width of mouseover for headers
            height: itemHeight_ + header.radius * 2

            color: "transparent"

            MouseArea{
                anchors.fill : parent
                hoverEnabled: true

                onDoubleClicked: {
                    view.doubleClicked(view.currentItem)
                }

                onClicked: {
                    if (hasChildren_ && mouseX < expandMouseArea.columnStart){
                        expanded = !expanded_;
                    } else {
                        selected = !selected_;
                        if(selected)
                        {
                            selectControl( copyableObject, false )
                        }
                        else
                        {
                            deselectControl( copyableObject, true )
                        }
                    }
                }

                onEntered: {
                    expandMouseArea.entered_  = true;
                }
                onExited: {
                    expandMouseArea.entered_ = false;
                }
            }
        }


        Rectangle {
            id: mouseHighlight //shaded fill for hovered rows that is under controls
            anchors.top: header.top
            anchors.left: header.left
            anchors.right: header.right

            height: hasChildren_ ? (itemHeight_ + itemMargin_ * 2) : itemHeight_

            anchors.margins: hasChildren_ ? 2 : 0
            radius: header.radius
            color: {
                if (selected_) {
                    return Qt.rgba(colorHighlight_.r,colorHighlight_.g,colorHighlight_.b,0.2);
                }
                else if (expandMouseArea.entered_) {
                    //white highlight for mouseover
                    return "#10FFFFFF";
                }
                else {
                    return "transparent";
                }
            }

        }

        Rectangle {
            id: mouseHighlightBorder //coloured line around selected row that is above controls
            anchors.fill: mouseHighlight
            anchors.margins: mouseHighlight.anchors.margins
            radius: header.radius
            color: "transparent"
            border.color: colorHighlight_
            z: 1
            visible: selected_ ? 1 : 0
        }

        Rectangle {
            id : body
            color : {
                var color = colorBody_[0];
                if (depth_ < colorBody_.length) {
                    color = colorBody_[depth_]
                } else {
                    color = colorBody_[colorBody_.length - 1]
                }
                return color
            }
            anchors.left : header.left
            anchors.right : header.right

            y : childItems.y + borderWidth_ * 2
            height : childItems.height - borderWidth_
            visible : expanded_
        }

        Rectangle {
            id : bodyEx
            color: body.color
            anchors.top : body.top
            anchors.bottom : body.bottom
            anchors.left : headerEx.left
            anchors.right : body.left
            visible : body.visible
        }

        Rectangle {
            id : indentLine
            anchors.top : body.top
            anchors.bottom : body.bottom
            anchors.left : body.left
            anchors.leftMargin : Math.min(2, indentation_)
            anchors.topMargin: itemMargin_
            anchors.bottomMargin: itemMargin_
            width: Math.min(splitterWidth_, indentation_ - 2)
            color: colorHighlight_
            visible : {
                var visible = false;
                if (depth_ < showIndentLine_.length && expanded_) {
                    visible = showIndentLine_[depth_];
                }
                return visible
            }
            Rectangle {
                anchors.top : parent.top
                anchors.bottom : parent.bottom
                anchors.left : parent.left
                width: parent.width / 2
                color: colorDarken_
                visible : parent.visible
            }
        }

        Rectangle {
            id : splitterHandle
            objectName: "splitterHandle"
            anchors.top : body.top
            anchors.bottom : body.bottom
            anchors.right : headerEx.left
            anchors.topMargin: itemMargin_
            anchors.bottomMargin: itemMargin_
            width: splitterShadow_ ? splitterWidth_ / 2 : splitterWidth_
            color: colorLighten_
            visible : {
                var visible = true;
                if (hasChildren_ && !expanded_) {
                    visible = false;
                }
                return visible
            }
            Rectangle {
                anchors.top : parent.top
                anchors.bottom : parent.bottom
                anchors.right : parent.left
                width: parent.width
                color: colorDarken_
                visible : splitterShadow_ ? parent.visible : false
            }
        }


        Item {
            id : content
            width : childrenRect.width
            height : childrenRect.height

            ColumnLayout {
                y : hasChildren_ ? borderWidth_ : 0

                spacing : 0

                Row {
                    Repeater {
                        model : WGSequenceList {
                            model : columnModel
                            sequence : columnSequence_
                        }
                        delegate : Row {
                            id : item
                            clip : hasChildren_ ? false : true
                            width : columnWidth( item, index )

                            Rectangle {
                                color : "transparent"
                                width : {
                                    var width = 0;
                                    if (expandButton.visible) {
                                        width += expandButton.x;
                                        width += expandButton.width;
                                    }
                                    else {
                                        width = itemMargin_;
                                    }
                                    return width;
                                }
                                height : itemHeight_
                                Image {
                                    id : expandButton
                                    objectName: "expandButton"
                                    visible : index == 0 && hasChildren_
                                    source : expanded_ ? "icons/arrow_down_16x16.png" : "icons/arrow_right_16x16.png"
                                    x : itemMargin_
                                    width : 16
                                    height : 16
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                            Loader {
                                width : {
                                    var width = parent.width;
                                    width -= x;				 //Left margin or the expand button
                                    width -= itemMargin_;	//Right margin
                                    width -= splitterWidth_; //Right splitter
                                    width -= Math.max( item.x + x + width - header.width, 0 ); // Clamp to the maximum width of the body
                                    return width;
                                }
                                height : itemHeight_
                                property variant itemData_ : model
                                sourceComponent : index < columnDelegates_.length ? columnDelegates_[index] : columnDelegate_
                            }
                        }
                    }
                }
                Row {
                    id : childItems

                    spacing : 0

                    Rectangle {
                        height : subTree.height
                        width : {
                            var width = itemMargin_;	   //Between left edge and body
                            width += indentation_;	  //Indentation
                            return width;
                        }
                        color : "transparent"
                    }
                    Loader {
                        id : subTree
                        width : {
                            var width = view.width;  //Parent width
                            width -= itemMargin_;	//Between left edge and body
                            width -= indentation_;   //Indentation
                            width -= itemMargin_;	//Between body and right edge
                            return width;
                        }
                        source : "TreeItem.qml"
                        onLoaded : {
                            children[0].depth_ = depth_ + 1
                        }
                    }
                }
            }
        }
    }
}
