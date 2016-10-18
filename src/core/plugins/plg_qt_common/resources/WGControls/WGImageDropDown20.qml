import QtQuick 2.5
import QtQuick.Controls 1.4
import Qt.labs.controls 1.0 as Labs
import Qt.labs.templates 1.0 as T
import QtQuick.Layouts 1.3
import WGControls 2.0

/*!
 \ingroup wgcontrols
 \brief A Qt.Labs Drop Down box with styleable menu that only shows an image in its collapsed state

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

WGDropDownBox {
    id: control
    objectName: "WGDropDownBox"
    WGComponent { type: "WGImageDropDown20" }

    implicitWidth: control.height + (showDropDownIndicator ? defaultSpacing.doubleMargin + defaultSpacing.standardMargin : 0)

    showDropDownIndicator: false

    // text that shows '?' if multipleValues or "Multiple Values" if enough size
    multiImageDelegate: Text {
        id: multiImageDelegate
        visible: multipleValues
        anchors.centerIn: parent

        text: control.width < control.multiValueTextMeasurement.width / 2 ? "?" : control.__multipleValuesString
        color: palette.textColor
        width: (control.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))

        font.italic: control.width < control.multiValueTextMeasurement.width / 2 ? false : true

        horizontalAlignment: "AlignHCenter"
        verticalAlignment: "AlignVCenter"
        wrapMode: Text.WordWrap
    }

    contentItem: Image {
        id: contentImage
        objectName: "ContentImage"
        anchors.centerIn: parent
        height: control.imageMaxHeight
        width: height
        Loader {
            anchors.centerIn: parent
            visible: control.imageRole
            sourceComponent: control.multipleValues ? multiImageDelegate : imageDelegate
        }
    }
}
