import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0

WGHotloadingBase {
    WGComponent { type: "WGHotloading" }

    height: 200
    radius: 5

    WGLabel {
        anchors.fill: parent
        verticalAlignment: TextInput.AlignVCenter
        horizontalAlignment: TextInput.AlignHCenter
        text: "Hotloaded Item"
        font.pointSize: 24
        font.bold: true
    }
}
