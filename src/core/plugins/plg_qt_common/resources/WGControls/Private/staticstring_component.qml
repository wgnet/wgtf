import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 1.0

import WGControls 2.0

import "qrc:///WGControls/wg_utils.js" as WGUtils

WGExpandingRowLayout {
    id: staticStringComponent
    objectName: itemData == null || typeof itemData.indexPath == "undefined" ? "staticstring_component" : itemData.indexPath
    anchors.left: parent ? parent.left : undefined
    anchors.right: parent ? parent.right : undefined
    width: childrenRect.width
    height: childrenRect.height
    enabled: itemData != null && itemData.enabled && !itemData.readOnly && (typeof readOnlyComponent == "undefined" || !readOnlyComponent)

    property var multipleValues: itemData != null && typeof itemData.value == "undefined" && itemData.multipleValues
    property var object: itemData == null ? null : itemData.object
    property var path: itemData == null ? "" : itemData.path
    property int value: itemData == null || (typeof itemData.value == "undefined" && itemData.multipleValues) ? 0 : itemData.value
    property string stringList: itemData != null && itemData.stringList ? itemData.stringList : ""
    property var stringListGenerator: function() { return internal.stringData ? internal.stringData.allStaticStringIds(object) : null }
    property var emptyStringId: internal.stringData ? internal.stringData.staticStringId("") : null

    QtObject {
        id: internal
        property var stringData: {
            return object != null ? object.getMetaObject(path, "StaticString") : null;
        }
        property var stringModel: ListModel {} //Model generated from string in mockup model
        property alias model: filteredModel.sourceModel // Model property from tableView
        property var idToInt: {} // id to index dict
        property bool mappingCreated: false
    }

    Dialog {
        id: stringSelectionDialog
        visible: false
        title: "Select string..."
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        width: 600
        height: 400
        property int initialValue: -1

        TextField {
            id: filterEdit
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            placeholderText: "filter"
        }

        TableView
        {
            id: tableView
            anchors.top: filterEdit.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height : stringSelectionDialog.height - 80
            focus: true
            TableViewColumn {
                role: "value"
                title: "String"
            }
            TableViewColumn {
                role: "key"
                title: "Id"
            }

            model: WGSortFilterProxy {
                id: filteredModel

                filterObject: QtObject {
                    property var filterExp: new RegExp(RegExp.escape(filterEdit.text), "i");

                    onFilterExpChanged: {
                        filteredModel.invalidateFilter();
                        tableView.selection.clear()
                    }

                    function filterAcceptsRow(item) {
                        return filterExp.test(item.value);
                    }
                }
            }

            onDoubleClicked: {
                var index = filteredModel.index(row, 0)
                setValueHelper(staticStringComponent, "value", itemKey(filteredModel.mapToSource(index).row))
                updateValue()
            }
        }

        onAccepted: {
            var index = filteredModel.index(tableView.currentRow, 0)
            setValueHelper(staticStringComponent, "value", itemKey(filteredModel.mapToSource(index).row))
            updateValue()
        }

        onRejected: {
            setValueHelper(staticStringComponent, "value", initialValue)
            updateValue()
        }
    }

    WGPushButton {
        id: button
        anchors.left: parent.left

        text: "..."
        implicitWidth: 30

        onClicked: {
            updateModel()
            updateMapping()
            var row = typeof internal.idToInt[value] == 'undefined' ? -1 : internal.idToInt[value]
            var index = filteredModel.sourceModel.index(row, 0)
            var filteredRow = filteredModel.mapFromSource(index).row
            tableView.currentRow = filteredRow
            tableView.selection.clear()
            tableView.selection.select(filteredRow)
            //TODO: fix view scrolls only on the second show
            tableView.positionViewAtRow(tableView.currentRow, ListView.Center)
            stringSelectionDialog.initialValue = value
            stringSelectionDialog.open()
        }
    }

    WGTextBox {
        id: textBox
        Layout.fillWidth: true
        readOnly: true
    }

    WGToolButton {
        visible: emptyStringId != null && typeof(emptyStringId) != "undefined"
        anchors.right: parent.right
        iconSource: "qrc:///WGControls/icons/delete_sml_16x16.png"
        onClicked: {
            textBox.text = "";
            setValueHelper(staticStringComponent, "value", emptyStringId)
            updateValue()
        }
    }

    Component.onCompleted: {
        updateModel();
        textBox.text = calcText();
    }

    onValueChanged: {
        textBox.text = calcText()
    }

    function updateValue() {
        if(!itemData.multipleValues) {
            itemData.value = staticStringComponent.value;
        } else {
            beginUndoFrame();
            itemData.value = staticStringComponent.value;
            endUndoFrame();
        }
    }

    function itemCount() {
        return internal.model == internal.stringModel ? internal.model.count : internal.model.count()
    }

    function itemKey(i) {
        return internal.model == internal.stringModel ? internal.model.get(i).key : internal.model.item(i).key
    }

    // Convert unsigned int to negative int value bitwise equal to unsigned
    // This hack is needed because qml only knows how to operate ints
    function uintToInt(i) {
        return i > 2147483647 ? i - 4294967296 : i
    }

    function calcText() {
        if(itemData == null) {
            return ""
        } else if (internal.stringData) {
            return internal.stringData.staticString(value)
        } else if (internal.model) {
            return internal.model.get(value).value
        }

        return -1;
    }

    function updateModel() {
        if (internal.model) {
            return
        }

        if (stringList && stringList.length > 0) {
            var strings = stringList.split("|");
            for (var i = 0; i < strings.length; ++i) {
                internal.stringModel.append({key: i, value: strings[i]});
            }
            internal.model = internal.stringModel
        }
        else if (stringListGenerator) {
            internal.model = stringListGenerator();
        }
        else {
            internal.model = null;
        }
    }

    function updateMapping() {
        if (internal.model == null) {
            return;
        }

        if (internal.mappingCreated) {
            return
        }

        internal.idToInt = {}

        var count = itemCount();
        for (var i = 0; i < count; ++i) {
            internal.idToInt[uintToInt(itemKey(i))] = i
        }

        internal.mappingCreated = true;
    }
}
