pragma Singleton
import QtQuick 2.5
import WGControls 2.0

Item {
    id: dialogs

    property var nativeFileDialog
    property var assetBrowserDialog
    property var customColorPickerDialog
    property var defaultColorPickerDialog

    WGComponent { type: "WGDialogs20" }

    Component.onCompleted: {
        nativeFileDialog = Qt.createQmlObject("import QtQuick 2.5; import WGControls 2.0; WGNativeFileDialog {}",dialogs);
        assetBrowserDialog = Qt.createQmlObject("import QtQuick 2.5; import WGControls 2.0; WGAssetBrowserDialog {}",dialogs);
        customColorPickerDialog = Qt.createQmlObject("import QtQuick 2.5; import WGControls 2.0; WGColorPickerDialog {}",dialogs);
        defaultColorPickerDialog = Qt.createQmlObject("import QtQuick 2.5; import WGControls 2.0; WGDefaultColorPickerDialog {}",dialogs);
    }
}
