import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0

/** Allows custom styling of all item views.
This includes derived views such as WGListView, WGTreeView, and WGTableView.
Use this to add pizazz to a view, by using custom drawn parts.
\ingroup wgcontrols */
Item {
    WGComponent { type: "WGItemViewStyle20" }
    
    /** Optional styling of the area behind a row.*/
    property Component rowBackground: Item {}
    /** Optional styling of the area behind the header row.*/
    property Component rowHeader: Item {}
    /** Optional styling of the area behind the footer row.*/
	property Component rowFooter: Item {}
    /** Optional styling of the area behind each column area in a row.*/
	property Component columnBackground: Item {}
    /** Optional styling of the area behind each column area in a header.*/
	property Component columnHeader: Item {}
    /** Optional styling of the area behind each column area om a footer.*/
	property Component columnFooter: Item {}
    /** Optional styling of the column resizing handles.*/
	property Component columnHandle: Rectangle {
        color: palette.darkColor
	}
}