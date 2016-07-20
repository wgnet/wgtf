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
    itemDelegate: Text
 {
        id: cell
     text: getText()
     color: styleData.textColor
     elide: Text.ElideRight

     function getText()
     {
            if(styleData.row < 0 || styleData.role === undefined || styleData.role === "")
                return "";
         try
         {
                var itemData = tableView.model.data(tableView.model.index(styleData.row), "value")
                // This call unfortunately makes adapting to a Javascript data model difficult
                return getProperty(itemData, styleData.role).toString()
         }
         catch(e)
         {
             //console.log(e.message)
             return ""
         }
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
            // Sometimes removing column 0 causes an exception, we need to continue running our code or we lose columns
            try{ removeColumn(0); }
            catch(e){}
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
        for(var i = 0; i < columns.count(); ++i)
        {
            var prop = columns.item(i)
            var title = prop
            if (typeof(prop) !== "string")
            {
                title = prop.title ? prop.title : "(???)"
                prop = prop.role ? prop.role : prop.toString()
            }
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
