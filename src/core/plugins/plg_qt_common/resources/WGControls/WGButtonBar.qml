import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

/*!
 \ingroup wgcontrols
 \brief A raised frame broken up into separate buttons with two small caps on either end.
 In theory it can use any style of button inside the buttonList with text and/or icons.
 By default it splits the bar up into even sized buttons (evenBoxes: true) and tries to make the bar large enough to fit all buttons if no width is provided.

Example:
\code{.js}
WGButtonBar {
    width: 200
    buttonList: [
        WGToolButton {
            text: "One"
        },
        WGToolButton {
            text: "Check"
            checked: true
            checkable: true
        },
        WGToolButton {
            iconSource: "icons/save_16x16.png"
        }
    ]
}
\endcode


Example to create a toggleable 'tag' style button with a close box:
\code{.js}
WGButtonBar {
    showSeparators: false
    evenBoxes: false
    buttonList: [
        WGPushButton {
            text: "Tag label"
            checkable: true
            checked: value.active
            style: WGTagButtonStyle{}
        },
        WGToolButton {
            iconSource: "icons/close_sml_16x16.png"
        }
    ]
}
\endcode
*/

WGButtonFrame {
    id: mainFrame
    objectName: "WGButtonFrame"
    WGComponent { type: "WGButtonBar" }

    /*! This property contains the list of objects, usually WGPushButtons, that will populate the buttonbar */
    property list <QtObject> buttonList

    /*!
        This property is used to define the label displayed used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*!
        This property defines whether the width alocated to each button is evenly distributed.
        The default value is \c true
        This may not leave enough room for the button label text if they are radically different sizes.
        Specify a larger width for the buttonBar itself to avoid this.
    */
    property bool evenBoxes: true

    /*!
        This property defines whether vertical lines will be used to split the buttons in the bar.
        Note: There will be unhighlighted gaps at either ends on mouseover/checked state even if this is set.
    */

    property bool showSeparators: true

    /*! \internal */
    property int __buttons : buttonList.length

    /*! \internal */
    property int __totalWidth: defaultSpacing.doubleMargin

    implicitHeight: defaultSpacing.minimumRowHeight

    //if Layout.preferredWidth is not defined, or set to -1, the button bar will use the total width of all buttons
    implicitWidth: __totalWidth

    Row {
        Rectangle {
            height: parent.height
            width: defaultSpacing.standardMargin
            color: "transparent"
        }

        Repeater {
            model: buttonList
            Rectangle {
                id: boxContainer
                width: {
                    if(!buttonList[index].visible) {
                        0
                    }
                    else {
                        if(evenBoxes)
                        {
                            (mainFrame.width - defaultSpacing.doubleMargin) / __buttons
                        }
                        else
                        {
                            buttonList[index].width
                        }
                    }

                }
                height: mainFrame.height
                color: "transparent"

                Component.onCompleted: {
                    if (typeof buttonList[index].text != "undefined" && typeof buttonList[index].iconSource != "undefined")
                    {
                        if(buttonList[index].visible) {
                            __totalWidth += buttonList[index].width
                        }
                        buttonList[index].parent = this
                        buttonList[index].anchors.fill = boxContainer
                        buttonList[index].radius = 0
                    }
                    else
                    {
                        boxContainer.color = "red"
                    }
                }

                // left most cap, and button separators
                WGSeparator {
                    visible: showSeparators
                    anchors.horizontalCenter: parent.left
                    height: mainFrame.height - defaultSpacing.doubleBorderSize
                    anchors.verticalCenter: parent.verticalCenter
                    vertical: true
                }
            }
        }
        //extra separator for the right end cap
        WGSeparator {
            visible: showSeparators
            height: mainFrame.height - defaultSpacing.doubleBorderSize
            anchors.verticalCenter: parent.verticalCenter
            vertical: true
        }
    }


    /*! Deprecated */
    property alias label_: mainFrame.label
}
