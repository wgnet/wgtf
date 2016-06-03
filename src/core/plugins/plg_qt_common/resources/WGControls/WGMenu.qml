import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0

/*!
 \brief A WG stylised menu

Example:
\code{.js}
    WGMenu {
        title: qsTr("File")

        MenuItem {
            text: qsTr("Exit")
            onTriggered: Qt.quit();
        }
    }
\endcode
*/

WGContextMenu {
    id: menu
    objectName: "WGMenu"
    path: "WGMenu"

    property var title: ""

    property list<QtObject> items
    default property alias itemsProperty : menu.items

    property var actions : []

    onAboutToShow : {
        for (var action in actions) {
            actions[action].active = true;
        }
    }

    onAboutToHide : {
        for (var action in actions) {
            actions[action].active = false;
        }
    }

    Component.onCompleted : {
        for (var i=0; i<items.length; i++) {
            createAction(items[i], path);
        }
    }

    Component.onDestruction : {
        for (var i=0; i<items.length; i++) {
            destroyAction(items[i], path);
        }
    }

    function insertItem(index, object) {
        createAction(object, path);
    }

    function removeItem(object) {
        destroyAction(object, path);
    }

    function createAction(object, menuPath) {
        //Automatically convert all MenuItem children into WGActions that we add to
        //the application just before we show the menu, and remove after we hide it
        var text = object.text;
        var trigger = object.trigger;
        if (typeof text !== "undefined" && typeof trigger !== "undefined") {
            var actionPath = menuPath + "|." + object.text;
            var qmlString = "import QtQuick 2.3; import QtQuick.Controls 1.2; import WGControls 1.0; ";
            qmlString += "WGAction { actionId: \"" + actionPath + "\"; }";
            var action = Qt.createQmlObject( qmlString, menu );
			actions.checkable = Qt.binding( function() { return object.checkable; } );
			actions.checked = Qt.binding( function() { return object.checked; } );
			actions.enabled = Qt.binding( function() { return object.enabled; } );
			actions.visible = Qt.binding( function() { return object.visible; } );
            action.triggered.connect(object.trigger);
            actions[actionPath] = action;
            return;
        }

        //Iterate all sub menu children for more MenuItems to convert to WGActions
        var title = object.title;
        var items = object.items;
        if (typeof title !== "undefined" && typeof items !== "undefined")
        {
            var subMenuPath = menuPath + "|." + title;
            for (var i=0; i<items.length; i++) {
                createAction(items[i], subMenuPath);
            }
        }
    }

    function destroyAction(object, menuPath) {
        var text = object.text;
        var trigger = object.trigger;
        if (typeof text !== "undefined" && typeof trigger !== "undefined") {
            var actionPath = menuPath + "|." + object.text;
            var action = actions[actionPath];
            action.destroy();
            actions[actionPath] = null;
            return;
        }

        var title = object.title;
        var items = object.items;
        if (typeof title !== "undefined" && typeof items !== "undefined")
        {
            var subMenuPath = menuPath + "|." + title;
            for (var i=0; i<items.length; i++) {
                destroyAction(items[i], subMenuPath);
            }
        }
    }
}
