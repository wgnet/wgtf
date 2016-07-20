import QtQuick 2.6
import QtQuick.Controls 1.2
import Qt.labs.controls 1.0 as Labs
import Qt.labs.templates 1.0 as T
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0
import WGControls 2.0

/*!
 \  brief Qt.Labs Drop Down box with styleable menu that can have a textRole and an imageRole

Example:
\code{.js}
WGDropDownBox {
    id: dropDown

    textRole: "label"
    imageRole: "icon"

    model: ListModel {}

    // Need to add items onCompleted or URL's won't work. C++ models should be fine.
    Component.onCompleted:
    {
        model.append({"label": "Option 1", "icon": Qt.resolvedUrl("icons/icon1.png")})
        model.append({"label": "Option 2", "icon": Qt.resolvedUrl("icons/icon2.png")})
        model.append({"label": "Option 3", "icon": Qt.resolvedUrl("icons/icon3.png")})
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

    /*! the property in the model that contains an image next to the text in the drop down.
    */
    property string imageRole: ""

    /*! the property in the model that determines if the control represents multiple values
    */
    property bool multipleValues: false

    /*! shows a tick icon next to the selected item in the menu
        The default value is true
    */
    property bool showRowIndicator: true

    /*! shows a small down arrow in the collapsed state to indicate a drop down
        The default value is true
    */
    property bool showDropDownIndicator: true

    /*! The maximum width of labels in the drop down menu.
        The default is set to the largest label in the menu but this can be overridden. If shorter than the largest label, text will elide.
    */
    property int labelMaxWidth: Math.max(textMetricsCreator.maxWidth, multiValueTextMeasurement.width)

    /*! The maximum height (and also therefore the width) of the image in both the collapsed and expanded state.
        The default is the height of the collapsed control with a small amount of padding for the borders.
    */
    property int imageMaxHeight: control.height - (defaultSpacing.doubleBorderSize * 2)

    /*! true if the collapsed control contains the mouse cursor */
    property bool hovered: wheelMouseArea.containsMouse

    /*! The string for the tooltip popup

        By default this is the textRole of the currentIndex*/
    property string tooltip: model.count > 0 && textRole ? model.get(currentIndex)[textRole] : ""

    /*! The QtObject for the icon/image in the dropDownBox
        By default this is an image that points to the imageRole URL but can be made any Item based QML object.
    */
    property Component imageDelegate: Image {
        id: imageDelegate
        source: control.imageRole && model.count > 0 ? model.get(control.currentIndex)[control.imageRole] : ""
        height: sourceSize.height < control.imageMaxHeight ? sourceSize.height : control.imageMaxHeight
        width: sourceSize.width < control.imageMaxHeight ? sourceSize.width : control.imageMaxHeight
        fillMode: Image.PreserveAspectFit
    }

    property Component multiImageDelegate: Text {
        id: multiImageDelegate
        visible: control.imageRole && multipleValues
        anchors.centerIn: parent
        text: "?"
        color: palette.textColor
        width: paintedWidth
    }

    property alias multiValueTextMeasurement: multiValueTextMeasurement

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.neutralTextColor

    /*! the property containing the string to be displayed when multiple values are represented by the control
    */
    property string __multipleValuesString: multipleValues ? "Multiple Values" : ""

    currentIndex: 0

    implicitHeight: defaultSpacing.minimumRowHeight ? defaultSpacing.minimumRowHeight : 22
    implicitWidth: labelMaxWidth + defaultSpacing.doubleMargin + defaultSpacing.minimumRowHeight
                   + (imageRole ? control.height + defaultSpacing.standardMargin : 0)
                   + (showDropDownIndicator ? defaultSpacing.doubleMargin + defaultSpacing.standardMargin : 0)

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

    TextMetrics {
        id: multiValueTextMeasurement
        text: __multipleValuesString
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
        id: listDelegate
        property string image: control.imageRole ? (Array.isArray(control.model) ? modelData[control.imageRole] : model[control.imageRole]) : ""
        parentControl: control
        width: Math.max(parentControl.labelMaxWidth + (parentControl.imageRole ? control.height : 0) + (showRowIndicator ? control.height : 0) + (defaultSpacing.doubleMargin * 2), control.width)
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        checkable: true
        autoExclusive: true
        checked: control.currentIndex === index
        highlighted: control.highlightedIndex === index
        pressed: highlighted && control.pressed
    }

    contentItem: Item {
            Item {
                id: contentImage
                objectName: "ContentImage"
                anchors.verticalCenter: parent.verticalCenter
                height: control.imageMaxHeight
                width: height
                visible: control.imageRole

                Loader {
                    anchors.centerIn: parent
                    sourceComponent: control.multipleValues ? multiImageDelegate : imageDelegate
                }
            }


            WGLabel {
                anchors.left: contentImage.visible ? contentImage.right : parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                anchors.leftMargin: contentImage.visible ? defaultSpacing.standardMargin : 0

                text: control.multipleValues ? __multipleValuesString : control.displayText
                font.italic: control.multipleValues ? true : false
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

            visible: showDropDownIndicator

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
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.pressed
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
                PropertyChanges {target: expandIcon; color: palette.darkShade}
            },
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightestShade}
            }
        ]
    }

    popup: T.Popup {
        id: popupbox
        objectName: "popupBox"
        y: control.height - 1
        implicitWidth: Math.max(labelMaxWidth + (imageRole ? control.height : 0) + (showRowIndicator ? control.height : 0) + (defaultSpacing.doubleMargin * 2), control.width)
        implicitHeight: Math.min(396, listview.contentHeight)
        topMargin: defaultSpacing.standardMargin
        bottomMargin: defaultSpacing.standardMargin

        contentItem: ListView {
            id: listview
            clip: true
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            T.ScrollIndicator.vertical: Labs.ScrollIndicator { id: scrollIndicator }
        }

        background: Rectangle {
            color: palette.lightPanelColor
            border.color: palette.darkestShade
            border.width: defaultSpacing.standardBorderSize
            radius: defaultSpacing.halfRadius


            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: defaultSpacing.standardBorderSize

                height: defaultSpacing.standardMargin

                visible: scrollIndicator.size < 1.0 && !listview.atYBeginning

                gradient: Gradient {
                    GradientStop { position: 0.0; color: palette.highlightShade }
                    GradientStop { position: 1.0; color: "transparent" }
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: defaultSpacing.standardBorderSize
                height: defaultSpacing.standardMargin

                visible: scrollIndicator.size < 1.0 && !listview.atYEnd

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 1.0; color: palette.highlightShade }
                }

            }
        }

    }

    MouseArea {
        id: wheelMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
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
        onExited: Tooltip.hideText()
        onCanceled: Tooltip.hideText()

        Timer {
            interval: 1000
            running: control.enabled && wheelMouseArea.containsMouse && !control.pressed && tooltip.length
            onTriggered: Tooltip.showText(wheelMouseArea, Qt.point(wheelMouseArea.mouseX, wheelMouseArea.mouseY), control.tooltip)
        }
    }



    /*! Deprecated */
    property alias label_: control.label
}
