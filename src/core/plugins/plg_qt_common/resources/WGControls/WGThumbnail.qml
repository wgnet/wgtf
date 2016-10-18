import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import WGControls 1.0

/*!
 \ingroup wgcontrols
 \brief Thumbnail image with default size and boder.
 Use WGThumbnailButton if thumbnail is meant as a button.
 Use WGThumbnailButton if thumbnail is editable.

Intended use:
Where a thumbnail of an asset is to be displayed.
 For example, in the asset browser

\code{.js}
WGThumbnail{
    parentControl: (id_of_parent_control)
    parentControlsMouseOverInfo: (id_of_parent_control).mouseOverInfo
}
\endcode
*/

Rectangle {
    id: thumbnail
    objectName: "WGThumbnail"
    WGComponent { type: "WGThumbnail" }

    /*! This property holds the location of the image to be displayed as the thumbnail
        The default value is false
    */
    property url source: ""

    /*! This property holds the default text string that will be displayed if /c source_ is not defined.
        The default value is \c "Image not found"
    */
    property string __notFoundString: "Image not found"

    property bool multipleValues: false

    property string __multipleValuesString: "Multiple Values"

    color: palette.lightShade

    implicitWidth: Math.min( icon.sourceSize.width + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 4)

    implicitHeight: Math.min( icon.sourceSize.height + defaultSpacing.doubleBorderSize, defaultSpacing.minimumRowHeight * 4)

    // support copy&paste
    WGCopyable {
        id: copyableControl

        WGCopyController {
            id: copyableObject

            onDataCopied : {
                setValue( thumbnail.source )
            }

            onDataPasted : {
                // readonly control
                console.log("ReadOnly Control WGThumbnail");
                //thumbnail.source_ = data
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

    Item {
        anchors.fill: parent
        Image {
            id: icon
            anchors.centerIn: parent
            height: sourceSize.height < parent.height ? sourceSize.height : parent.height
            width: sourceSize.width < parent.width ? sourceSize.width : parent.width
            anchors.margins: defaultSpacing.standardBorderSize
            source: thumbnail.source
            visible: !multipleValues
            fillMode: Image.PreserveAspectFit
        }
        WGLabel{
            objectName: "Label"
            id: missingText
            anchors.centerIn: parent
            width: (parent.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: __notFoundString
            font.pointSize: thumbnail.height <= defaultSpacing.minimumRowHeight ? 5 : 9
            visible: thumbnail.source == "" && !multipleValues
            wrapMode: "WordWrap"
        }
        WGLabel{
            objectName: "MultiValuesText"
            id: multipleValuesText
            anchors.centerIn: parent
            width: (parent.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: __multipleValuesString
            visible: multipleValues
            wrapMode: "Wrap"
        }
    }

    /*! Deprecated */
    property alias source_: thumbnail.source
}
