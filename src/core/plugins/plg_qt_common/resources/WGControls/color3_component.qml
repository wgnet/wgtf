import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2


Loader
{
    id: color3_Loader
    objectName:  itemData != null ? itemData.indexPath : "color3_component"
    Component.onCompleted: {
        color3_Loader.setSource("color4_component.qml", { "showAlpha": false });
    }
}
