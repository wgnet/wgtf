import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0

import "wg_hotloading.js" as WGHotloadingJS

Rectangle {
    WGComponent { type: "WGHotloadingBase" }
    
    border.width: 2
    border.color: WGHotloadingJS.getBorderColor()
    
    gradient: Gradient {
        GradientStop { position: 0.0; color: "darkgray" }
        GradientStop { position: 1.0; color: "steelblue" }
    }
}
