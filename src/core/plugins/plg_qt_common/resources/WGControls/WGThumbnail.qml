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
    source: "path/example.png"
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

    /*! This value indicates if the control represents multipleValues
      */

    /*! This value indicates if the control represents multipleValues
      */
    property bool multipleValues: false

    /*! This property holds the default text string that will be displayed if multipleValues is true.
    */
    property string __multipleValuesString: "Multiple Values"

    property alias icon: icon

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
        WGImage {
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
            objectName: "MultiValuesText"
            id: multipleValuesText
            anchors.centerIn: parent
            width: (parent.width - (defaultSpacing.leftMargin + defaultSpacing.rightMargin))
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            text: __multipleValuesString
            font.pointSize: thumbnail.height <= 32 ? 5 : 9
            visible: multipleValues
            wrapMode: "WordWrap"
        }
    }

    /*! Deprecated */
    property alias source_: thumbnail.source
}
