import QtQuick 2.5
import QtQuick.Controls 1.4
import WGControls 2.0
import WGControls.Styles 2.0

/*!
 \ingroup wgcontrols
 \brief Text field intended for file url.

Example:
\code{.js}
WGFileUrl {
    placeholderText: "Placeholder Text"
    Layout.fillWidth: true
}
\endcode
*/

WGTextBox {
    id: fileUrl
    WGComponent { type: "WGFileUrl20" }

    /*! This function calculates the implicitWidth using just the file and extension
    */
    function recalculateFileNameWidth() {
        var fileString = fileUrl.text
        var fileName = fileString.substr(fileString.lastIndexOf('/') + 1);
        contentLengthHelper.text = fileName + "MM"
    }

    implicitWidth: contentLengthHelper.contentWidth + defaultSpacing.doubleMargin

    Text {
        id: contentLengthHelper
        visible: false
        Component.onCompleted: {
            recalculateFileNameWidth()
        }
    }

    onEditAccepted: {
        recalculateFileNameWidth()
    }

    onEditCancelled: {
        recalculateFileNameWidth()
    }
}
