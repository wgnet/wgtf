import QtQuick 2.6
import QtQuick.Controls 1.2
import Qt.labs.controls 1.0 as Labs
import Qt.labs.templates 1.0 as T
import QtQuick.Layouts 1.3
import WGControls 1.0

/*!
 \  brief Qt.Labs Drop Down box with styleable menu that can have a textRole and an imageRole

Example:
\code{.js}
WGDropDownBox {
    id: dropDown

    textRole: "label"
    imageRole: "icon"

    model: ListModel {
        ListElement { label: "Option 1", icon: "icons/icon1.png"}
        ListElement { label: "Option 2", icon: "icons/icon2.png"}
        ListElement { label: "Option 3", icon: "icons/icon3.png"}
    }
}
\endcode
*/

Labs.ComboBox {
    id: control
    objectName: "WGDropDownBox"

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.neutralTextColor

    // the property in the model that contains an image next to the text in the drop down.
    property string imageRole: ""

    currentIndex: 0

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22
    implicitWidth: textMetricsCreator.maxWidth + defaultSpacing.doubleMargin * 2 + defaultSpacing.minimumRowHeight
                   + (imageRole ? control.height + defaultSpacing.standardMargin : 0)

    //find the widest text in model to help set control width
    Repeater {
        id: textMetricsCreator
        model: control.model
        property real maxWidth: 0

        Item {
            id:itemWrapper

            TextMetrics {
                id: fakeText
                text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
                onTextChanged: {
                    textMetricsCreator.maxWidth = Math.max(textMetricsCreator.maxWidth, width)
                }
            }
        }
    }

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( control.currentIndex )
            }

            onDataPasted : {
                control.currentIndex = data
            }
        }

        onSelectedChanged : selected ? selectControl( copyableObject ) : deselectControl( copyableObject )
    }

    delegate: WGDropDownDelegate {
        property string image: control.imageRole ? (Array.isArray(control.model) ? modelData[control.imageRole] : model[control.imageRole]) : ""
        width: control.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        checkable: true
        autoExclusive: true
        checked: control.currentIndex === index
        highlighted: control.highlightedIndex === index
        pressed: highlighted && control.pressed
    }

    contentItem: Item {
        Image {
            id: contentImage
            anchors.verticalCenter: parent.verticalCenter
            source: control.imageRole ? model.get(control.currentIndex)[control.imageRole] : ""
            visible: control.imageRole
            height: control.height - defaultSpacing.doubleBorderSize
            width: height
        }

        WGLabel {
            anchors.left: contentImage.visible ? contentImage.right : parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: contentImage.visible ? defaultSpacing.standardMargin : 0

            text: control.displayText
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }

    background: Item {
        implicitWidth: 120
        implicitHeight: defaultSpacing.minimumRowHeight

        WGButtonFrame {
            id: buttonFrame
            width: parent.width
            height: parent.height
        }

        Text {
            id: expandIcon
            color : control.__textColor

            x: parent.width - width - control.rightPadding
            y: (parent.height - height) / 2

            font.family : "Marlett"
            font.pixelSize: parent.height / 2
            renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering
            text : "\uF075"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: buttonFrame; color: palette.darkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightestShade}
            }
        ]
    }

    popup: T.Popup {
        y: control.height - 1
        implicitWidth: control.width
        implicitHeight: Math.min(396, listview.contentHeight)
        topMargin: 6
        bottomMargin: 6

        contentItem: ListView {
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            Item {
                z: 10
                parent: listview
                width: listview.width
                height: listview.height
            }

            T.ScrollIndicator.vertical: Labs.ScrollIndicator { }
        }

        background: Rectangle {
            color: palette.lightPanelColor
            border.color: palette.darkestShade
            border.width: defaultSpacing.standardBorderSize
            radius: defaultSpacing.halfRadius
        }
    }

    MouseArea {
        id: wheelMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: {
            if (control.activeFocus || control.popup.visible)
                {
                if (wheel.angleDelta.y > 0 && control.currentIndex > 0)
                {
                    control.currentIndex -= 1 ;
                } else if (wheel.angleDelta.y < 0 && control.currentIndex < control.count - 1)
                {
                    control.currentIndex += 1
                }
            }
        }
    }

    /*! Deprecated */
    property alias label_: control.label
}
