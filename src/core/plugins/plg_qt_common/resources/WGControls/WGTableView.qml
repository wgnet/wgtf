import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

// TODO: This could be used as a starting point for the WGGridEdit/WGTableView Excel like spreadsheet 
// There are a couple of advantages TableView has over GridView, such as resizable movable columns
// Either way custom cell selection will be required.
TableView
{
    id: tableView
    Layout.fillWidth: true
    itemDelegate: Item
    {
        id: item
        Component.onCompleted:{
            var column = getColumn(styleData.column)
            // This is just a place holder for now allowing us to visualize the data
            var snippet =
'import QtQuick 2.5
 import QtQuick.Controls 1.3
 Text
 {
     text: getText()
     color: styleData.textColor
     elide: Text.ElideRight

     function getText()
     {
         var itemData = tableView.model.data(tableView.model.index(' + styleData.row + '), "Value")
         try
         {
              return itemData.' +  column.role + '.toString()
         }
         catch(e)
         {
             //console.log(e.message)
             return ""
         }
     }
 }'
            internal.dynamicObject = Qt.createQmlObject(snippet,
                item, "dynamicTableViewItemDelegate")
        }
        Component.onDestruction:
        {
            internal.dynamicObject.destroy()
        }

        QtObject
        {
            id: internal
            property var dynamicObject: undefined
        }
    }

    property var fixedColumns: []
    property var movableColumns: []

    property Component columnDelegate: TableViewColumn
    {
        resizable: true
    }

    onFixedColumnsChanged: {
        updateColumns()
    }

    onMovableColumnsChanged: {
        updateColumns()
    }

    function updateColumns()
    {
        while(columnCount > 0)
        {
            removeColumn(0);
        }
        addColumns(fixedColumns, false)
        addColumns(movableColumns, true)
    }

    function addColumns(columns, movable)
    {
        if(columns === undefined)
            return

        // How I wish we supported collections properly in Qml using
        // QVariantList and QVariantMap to auto convert to Javascript types
        // and allow for interchanging javascript dummy data lists with C++ data
        // See http://doc.qt.io/qt-5/qtqml-cppintegration-data.html

        //for(var i=0; i < columns.length; ++i)
        //{
        //    var prop = columns[i]
        //    var title = prop
        //    if (typeof(prop) !== "string")
        //        title = prop.title ? prop.title : prop.toString()
        //        prop = prop.role ? prop.role : prop.toString()
        //    var column = columnDelegate.createObject(undefined,
        //        {
        //            "role": prop,
        //            "title": title,
        //            "movable": movable
        //        })
        //    addColumn(column)
        //}

        // Iterate through the list of columns
        var iter = iterator(columns)
        while(iter.moveNext())
        {
            var prop = iter.current
            var title = prop
            if (typeof(prop) !== "string")
                title = prop.title ? prop.title : "(???)"
                prop = prop.role ? prop.role : prop.toString()
            var column = columnDelegate.createObject(undefined,
                {
                    "role": prop,
                    "title": title,
                    "movable": movable
                })
            addColumn(column)
        }
    }
}
