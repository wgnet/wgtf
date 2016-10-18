import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import WGControls 1.0
import WGControls.Styles 1.0

/*!
 \ingroup wgcontrols
 \brief Button that has an variable image as an icon.
 Intended for textures etc.

\code{.js}
WGThumbnailButton {
    source: "icons/file"
    defaultText: "Click to Load an Image"
}\endcode
*/

Button {
    id: thumbnailButton
    objectName: "WGThumbnailButton"
    WGComponent { type: "WGThumbnailButton" }

    /*! This property contains the default text string that will be shown when \c iconSource: is an empty string.
        The default value is \c "Default text has not been set"*/
    property string defaultText: "Default text has not been set"

    /*! This property is not used by anything */
    //TODO: remove this property it appears to be used by nothing
    property int radius: defaultSpacing.standardRadius

    /*! This property is used to define the buttons label when used in a WGFormLayout
        The default value is an empty string
    */
    property string label: ""

    /*! This property holds the url of the thumbnail displayed on the button
        The default value is an empty string
    */
    iconSource: ""

    property alias source: icon.source

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( thumbnailButton.iconSource )
            }

            onDataPasted : {
                thumbnailButton.iconSource = data
            }
        }

        onSelectedChanged : {
            if(selected)
            {
                selectControl( copyableObject )
            }
            else
            {
                deselectControl( copyableObject )
            }
        }
    }

    implicitWidth: {
        defaultSpacing.minimumRowHeight * 4
    }

    implicitHeight: {
        defaultSpacing.minimumRowHeight * 4
    }

    onClicked: {
        if (enabled == true){
            fileDialog.visible = true
        }
    }

    style: WGButtonStyle{

    }

    Item {
        anchors.fill: parent
        Image {
            id: icon
            anchors.fill: parent
            anchors.margins: defaultSpacing.standardMargin
            opacity: enabled ? 1 : 0.4

            fillMode: Image.PreserveAspectFit
        }

        WGLabel{
            objectName: "Label"
            id: defaulttext1
            anchors.centerIn: parent
            width: (parent.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: defaultText
            visible: icon.source == ""
            wrapMode: "Wrap"
        }
    }

    FileDialog {
        objectName: "FileDialog"
        id: fileDialog
        title: "Choose a texture"
        visible: false
        nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]
        onAccepted: {
            icon.source = fileDialog.fileUrl
            defaulttext1.visible = false
        }
    }

    /*! Deprecated */
    property alias label_: thumbnailButton.label
}
