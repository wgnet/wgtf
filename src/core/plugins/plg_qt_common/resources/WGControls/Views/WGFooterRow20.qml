import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls.Private 2.0

/** Shared component for the list of column footers in the WGItemViews.
\ingroup wgcontrols */
Item {
    id: itemRow
    objectName: "WGFooterRow"
    WGComponent { type: "WGFooterRow20" }

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024
	
    /** This is a link to the containing WGItemView.*/
	property var view

    /* MOVE INTO STYLE*/
    Rectangle {
        id: backgroundArea
        anchors.fill: row
        color: palette.midDarkColor
        opacity: 1
        visible: true
    }
    /**/

    Row {
        id: row
		spacing: view.columnSpacing

        Repeater {
            model: columnCount()

            Item {
                id: footerItem
                width: view.columnWidths[index]
                height: childrenRect.height
                clip: true

                /** Loader of the custom footer cell component.*/
                Loader {
                    id: columnDelegateLoader
                    sourceComponent: view.getFooterDelegate(index)

                    /** Provides the custom footer cell component access to header roles.*/
                    property var headerData: view.columnSequence.length <= index ? view.headerData[index] :  view.headerData[view.columnSequence[index]]
					/** Exposes current column index to the custom cell component.*/
                    property int headerColumnIndex: index

                    onLoaded: item.parent = footerItem
                }
            }
        }
    }
}

