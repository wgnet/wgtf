import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 2.0

WGImage {
    id: assetIcon
    height: iconSize
    width: iconSize
    opacity: isHidden ? 0.5 : 1.0
    fillMode: Image.PreserveAspectFit
    asynchronous: true
	WGComponent { type: "WGAssetIcon20" } 

    /*! This property determines the pixel size of the icon file to choose */
    property string iconFileSize: (iconSize <= 24 ? "16" : (iconSize <= 64 ? "48" : ""))

    source: iconProvider.getIconSource(asset, isDirectory, iconFileSize)

    Layout.preferredHeight: iconSize
    Layout.preferredWidth: iconSize
    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
    smooth: false

    property var asset: null
    property var iconProvider: null
    property int iconSize: 16
    property string readOnlyIcon: iconSize > 32 ? "file_readonly16" : "file_readonly"
    property string compressedFolderIcon: iconSize > 32 ? "folder_packed16" : "folder_packed"
    property string compressedFileIcon: iconSize > 32 ? "file_packed16" : "file_packed"

    property bool showOverlays: true

    readonly property bool assetValid: typeof asset != "undefined" && asset != null
    readonly property bool isDirectory: assetValid && typeof asset.isDirectory !== "undefined" ? asset.isDirectory : false
    readonly property bool isCompressed: assetValid && typeof asset.compressed !== "undefined" ? asset.compressed : false
    readonly property bool isReadOnly: assetValid && typeof asset.readOnly !== "undefined" ? asset.readOnly : false
    readonly property bool isHidden: assetValid && typeof asset.isHidden !== "undefined" ? asset.isHidden : false
    // TODO: Make status overlays general purpose instead of perforce specific
    readonly property bool p4StatusValid: assetValid && typeof asset.p4Status !== "undefined" && asset.p4Status !== null
    readonly property var p4ServerStatus: p4StatusValid ? asset.p4Status : null
    readonly property var p4StatusIcons: p4StatusValid && p4ServerStatus !== null && typeof p4ServerStatus.statusIcons !== "undefined" ? p4ServerStatus.statusIcons : null
    readonly property bool p4Unknown: p4StatusValid && p4ServerStatus !== null &&  typeof p4ServerStatus.hasUnknownState !== "undefined" && p4ServerStatus.hasUnknownState

    QtObject {
        id: internal

        function p4TopLeftIconsShown(p4StatusIcons, p4ServerStatus, iconOverlay)
        {
            if (p4StatusIcons === null) {
                return false;
            }

            var statusIconCount = p4StatusIcons.count();

            for (var i = 0; i < statusIconCount; ++i) {
                var statusIcon = p4StatusIcons.value(i);
                var position = statusIcon != null ? p4ServerStatus.statusIconPosition(statusIcon) : 0;

                if (position === iconOverlay.topLeftCorner) {
                    return true;
                }
            }

            return false;
        }
    }

    WGIconOverlay {
        id: compressedIconOverlay
        source: isDirectory ? compressedFolderIcon : compressedFileIcon
        icon: assetIcon
        visible: isCompressed && showOverlays
        position: bottomRightCorner
        x: 1
    }

    WGIconOverlay {
        id: readOnlyIconOverlay
        source: readOnlyIcon
        icon: assetIcon
        visible:  !p4Unknown && !isCompressed && isReadOnly && !isDirectory && !p4OverlayShown && showOverlays
        position: topLeftCorner

        property var p4OverlayShown: internal.p4TopLeftIconsShown(p4StatusIcons, p4ServerStatus, readOnlyIconOverlay)
        x: 1
    }

    Repeater {
        id: p4IconOverlays
        model: isDirectory || !p4StatusValid || p4StatusIcons === null ? 0 : p4StatusIcons

        WGIconOverlay {
            id: p4iconLoader
            source: iconSize > 32 ? value + "16" : value
            icon: assetIcon
            position: value != null ? p4ServerStatus.statusIconPosition(value) : 0
            x: 1
            visible: showOverlays
        }
    }
}
