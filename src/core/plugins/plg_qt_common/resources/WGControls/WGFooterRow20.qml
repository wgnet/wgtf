import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0


Item {
    id: itemRow
    objectName: "WGFooterRow"

    width: childrenRect.width != 0 ? childrenRect.width : 1024
    height: childrenRect.height != 0 ? childrenRect.height : 1024
	
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
                width: view.columnWidths[index]
                height: childrenRect.height
                clip: true

                // header component
                Loader {
                    id: columnDelegateLoader
                    property var headerData: view.columnSequence.length <= index ? view.headerData[index] :  view.headerData[view.columnSequence[index]]
                    property var headerWidth: parent.width
                    sourceComponent: view.footerDelegates[index]
                }
            }
        }
    }
}

