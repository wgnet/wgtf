import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Private 1.0
import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0
import WGControls.Private 2.0

WGPanel
{
    id: toolBoxPanel
    title: "Toolbox"
    layoutHints: { 'right': 1.0 }
    objectName: "toolBoxPanel"
    WGComponent { type: "toolbox_panel" }
    // TODO NGT-2493 ScrollView steals keyboard focus
    Keys.forwardTo: [toolPropertyView]
    property var buttonSize : 40

    property var sourceModel: activeTool

    property color darkerGradientColor: Qt.darker(palette.highlightColor, 3.0)
    property color lighterGradientColor: Qt.lighter(palette.highlightColor, 1.3)

    color: palette.mainWindowColor

    Gradient
    {
        id: buttonGradient
        GradientStop { position: 0.0; color: palette.darkestShade }
        GradientStop { position: 1.0; color: palette.darkColor }
    }

    Gradient
    {
        id: activeButtonGradient
        GradientStop { position: 0.0; color: darkerGradientColor }
        GradientStop { position: 0.5; color: palette.highlightColor }
        GradientStop { position: 1.0; color: lighterGradientColor }
    }

    WGColumnLayout
    {
        id: layout
        anchors.fill: parent

        RowLayout
        {
            Layout.minimumHeight: buttonSize + 2
            Layout.maximumHeight: buttonSize + 2
            spacing: 1

            Rectangle
            {
                anchors.fill: parent
                color: palette.darkestShade
            }

            Repeater
            {
                id: nodesRepeater
                model: tools

                delegate: Item
                {
                    width: buttonSize
                    height: buttonSize


                    Rectangle
                    {
                        visible: true
                        anchors.fill: parent
                        gradient: value.active ? activeButtonGradient : buttonGradient
                    }
                    ToolButton
                    {
                        anchors.fill: parent
                        anchors.margins: 2
                        visible: true
                        enabled: value.canActivate

                        Image
                        {
                            source: value.icon
                            anchors.fill: parent
                        }

                        Rectangle
                        {
                            visible: !value.canActivate
                            anchors.fill: parent
                            color: "#80888888"
                        }

                        tooltip: value.tooltip
                        onClicked:
                        {
                            activateTool(index);
                        }
                    }
                }
            }
        }

        WGScrollView {
            anchors.margins: 10
            Layout.fillWidth: true
            Layout.fillHeight: true

            WGPropertyTreeView {
                id: toolPropertyView
                model: sourceModel
                clamp: true

                style: WGToolboxTreeViewStyle
                {
                    customView: toolPropertyView.view
                }

                propertyDelegate: Item {
                        id: propertyComponentParent
                        height: subMode ? subModeLabel.height : (delegateLoader.status == Loader.Ready ? delegateLoader.item.height : defaultSpacing.minimumRowHeight)
                        width: parent != null ? parent.width : 0
                        implicitWidth: subMode ? subModeLabel.implicitWidth : (delegateLoader.status == Loader.Ready ? delegateLoader.item.implicitWidth : 0)

                        property var subMode: itemDepth == 0 && itemData != null && itemData.definitionModel != null
                        property bool rowCurrent: rowIsCurrent

                        Component.onCompleted: {
                            getImplicitWidths = function() {
                                return Qt.vector3d(implicitWidth, implicitWidth, -1);
                            }
                        }

                        onRowCurrentChanged: {
                            if (rowCurrent && delegateLoader.item != null)
                            {
                                delegateLoader.item.forceActiveFocus()
                            }
                        }

                        Connections {
                            target: delegateLoader.item
                            onActiveFocusChanged: {
                                if (delegateLoader.item.activeFocus && !propertyComponentParent.rowCurrent)
                                {
                                    toolPropertyView.selectionModel.setCurrentIndex(itemRowModelIndex, 0)
                                }
                            }
                        }

                        Loader {
                            id: delegateLoader
                            width: parent.width
                            active: !subMode
                            property bool readOnlyComponent: toolPropertyView.readOnly
                            sourceComponent: itemData != null ? itemData.component : null
                        }

                        Text {
                            id: subModeLabel
                            text: {
                                var polyModel = itemData != null ? itemData.definitionModel : null
                                var definition = itemData != null ? itemData.definition : null
                                var count = (polyModel != null) ? polyModel.count() : 0;
                                for (var i = 0; i < count; ++i) {
                                    var item = polyModel.item(i);
                                    if (definition == item.value) {
                                        return item.display
                                    }
                                }
                                return "!NO LABEL!";
                            }
                            font.bold: true
                            color: palette.textColor
                            verticalAlignment: Text.AlignVCenter
                            height: defaultSpacing.minimumRowHeight
                            visible: subMode
                        }
                    }

                columnDelegates: [labelDelegate, propertyDelegate]
            }
        }
    }
}
