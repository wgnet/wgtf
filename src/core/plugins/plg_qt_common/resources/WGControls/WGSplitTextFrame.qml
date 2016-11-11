import QtQuick 2.5
import QtQuick.Layouts 1.3
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief A Frame that can be broken up into X textboxes
 Can use separators or decimal places between each box

Example:
\code{.js}
WGSplitTextFrame {
    characterSeparator: true
    height: 24
    Layout.preferredWidth: 130
    label: "IP Address:"

    boxList: [
        WGNumberBox{
            value: 192
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 168
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 0
            minimumValue: 0
            maximumValue: 255
        },
        WGNumberBox{
            value: 1
            minimumValue: 0
            maximumValue: 255
        }
    ]
}
\endcode
*/

WGTextBoxFrame {
    id: mainFrame
    objectName: "WGSplitTextFrame"
    WGComponent { type: "WGSplitTextFrame" }

    /*! This property contains the list of textboxes to be displayed in the frame */
    property list<QtObject> boxList

    /*! This property toggles use of a character instead of a line between fields
        The default value is \c false
    */
    property bool characterSeparator: false

    /*! This text character used for the characterSeparator. Recommended a single character only.
        The default value is \c "."
    */
    property string separatorCharacter: "."

    /*! \internal */
    property int __totalBoxes : boxList.length

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! \internal */
    property int __totalWidth: 0 //calculates total width based on number of boxes

    /*!
        This property defines whether the width alocated to each box is evenly distributed.
        The default value is \c true
    */
    property bool evenBoxes: true

    property Component invisibleStyle: WGInvisTextBoxStyle {}

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22

    implicitWidth: boxList.length === 0 ? warningtext.contentWidth + defaultSpacing.standardMargin * 2 : 40 * boxList.length

    Layout.preferredWidth: __totalWidth == 0 ? warningtext.contentWidth + defaultSpacing.standardMargin * 2 : __totalWidth

    width: __totalWidth == 0 ? warningtext.contentWidth + defaultSpacing.standardMargin * 2: __totalWidth

    // Placeholder text prevents zero size UI components
    Text {
        id: warningtext
        text: "Warning: WGSplitTextFrame has no boxList"
        visible: boxList.length == 0
        color: "white"
        anchors.left: mainFrame.left
        anchors.bottom: mainFrame.bottom
        anchors.margins: defaultSpacing.standardMargin
    }

    Row {
        Repeater {
            model: boxList
            Rectangle {
                id: boxContainer
                color: "transparent"

                height: mainFrame.height

                width: {
                    if(evenBoxes)
                    {
                        Math.round(mainFrame.width / Math.max(boxList.length, 1))
                    }
                    else
                    {
                        model.modelData.width
                    }
                }

                Component.onCompleted: {
                    if(boxList[index].text != undefined)
                    {
                        __totalWidth += boxList[index].width
                        boxList[index].style = invisibleStyle
                        boxList[index].horizontalAlignment = Text.AlignHCenter
                        boxList[index].parent = this
                        boxList[index].anchors.fill = boxContainer
                    }
                    else if (boxList[index].value != undefined)
                    {
                        __totalWidth += boxList[index].width
                        boxList[index].hasArrows = false
                        boxList[index].textBoxStyle = invisibleStyle
                        boxList[index].horizontalAlignment = Text.AlignHCenter
                        boxList[index].parent = this
                        boxList[index].anchors.fill = boxContainer
                    }
                    else
                    {
                        boxContainer.color = "red"
                    }
                }

                WGLabel {
                    id: decimalPoint
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    y: 3
                    text: separatorCharacter
                    visible: index != 0 && characterSeparator ? true : false
                }

                WGSeparator {
                    id: internalBorder
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    anchors.verticalCenter: parent.verticalCenter
                    vertical: true

                    //first separator is invisible
                    visible: index != 0 && !characterSeparator ? true : false
                }
            }
        }
    }

    /*! Deprecated */
    property alias decimalSeparator: mainFrame.characterSeparator

    /*! Deprecated */
    property alias totalBoxes: mainFrame.__totalBoxes

    /*! Deprecated */
    property alias totalWidth: mainFrame.__totalWidth

    /*! Deprecated */
    property alias label_: mainFrame.label
}
