import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2


Loader
{
    id: color3_Loader
    objectName: typeof itemData.indexPath == "undefined" ? "color3_component" : itemData.indexPath
    Component.onCompleted: {
        color3_Loader.setSource("color4_component.qml", { "showAlpha": false });
    }
}
