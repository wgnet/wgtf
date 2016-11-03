import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import WGControls 1.0

/*!
    \ingroup wgcontrols
    \brief Provides custom styling for WGTabView.
*/

TabViewStyle {
    objectName: "WGTabViewStyle"
    WGComponent { type: "WGTabViewStyle" }

    tabsMovable: true
    frameOverlap: 0

    /*! This property defines the height of the selected tab
        The default value is \c 22
    */
    property int tabHeight: 22

    /*! This property defines the height difference between the selected an non selected tabs
        The default value is \c 2
    */
    property int tabHeightDiff: 2

    /*! This property defines the minimum width of tab
        The default value is \c 60
    */
    property int tabMinWidth: 60

    /*! This property defines the width of the gap between tabs
        The default value is \c 0
    */
    property int tabSpacer: 0

    /*! This property defines the combined width of the padding left and right of the tab title to the tab border
        The default value is \c 10
    */
    property int tabTextSpacer: 10

    tab: Rectangle { //The space encompassed by the tabs including gap and top
            id: tabSpace
            color: "transparent"
            implicitWidth: Math.max(text.width + tabTextSpacer + tabSpacer, tabMinWidth + tabSpacer)
            implicitHeight: tabHeight

            Rectangle { //Used to anchor the tabs to the bottom
                id: visualTabSpace
                color: "transparent"
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                implicitWidth: Math.max(text.width + tabTextSpacer, tabMinWidth)
                implicitHeight: tabHeight

                Rectangle { //The visible tab, only the top half shown, rest obscured.
                    id: tabTopHalf
                    anchors.bottom: (tabPosition == Qt.TopEdge)? parent.bottom : undefined
                    anchors.top: (tabPosition == Qt.BottomEdge)? parent.top : undefined
                    anchors.left: parent.left
                    color: styleData.selected ? palette.midLightColor : palette.midDarkColor
                    implicitWidth: Math.max(text.width + tabTextSpacer, tabMinWidth)
                    implicitHeight: styleData.selected ? tabHeight :  tabHeight -  tabHeightDiff

                    Rectangle { //Bottom half of the tab for obscuring rounded bottoms of tab
                        id: tabBottomHalf
                        anchors.top: parent.verticalCenter
                        color: styleData.selected ? palette.midLightColor : palette.midDarkColor
                        implicitWidth: Math.max(text.width + tabTextSpacer, tabMinWidth)
                        implicitHeight: ((tabPosition == Qt.TopEdge) ? Math.floor(parent.height / 2) : (Math.floor(parent.height / 2)-1))
                    }
                    Text {
                        id: text
                        anchors.centerIn: parent
                        text: styleData.title
                        color: styleData.selected ? palette.textColor : palette.disabledTextColor
                    }
                    Rectangle { //highlight line at top of tab
                        id: topTabHighlight
                        anchors.top: parent.top
                        implicitHeight: 1
                        implicitWidth: Math.max(text.width + tabTextSpacer, tabMinWidth)
                        color: (tabPosition == Qt.BottomEdge)?( "transparent") : (styleData.selected ? palette.lightestShade : palette.lighterShade)
                    }
                    Rectangle { //highlight remover line at bottom of tab if not selected
                        id: bottomTabHighlight
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        implicitHeight: 1
                        implicitWidth: parent.width
                        color: (tabPosition == Qt.BottomEdge)?(styleData.selected ? palette.darkerShade: palette.darkShade) : (styleData.selected ? "transparent" : palette.lightestShade)
                    }
                    Rectangle { //Shading on the right side of a tab
                        id: rightTabShading
                        anchors.right: parent.right
                        anchors.top: (tabPosition == Qt.TopEdge)? topTabHighlight.top : topTabHighlight.top
                        implicitHeight: styleData.selected ? parent.height -0 : ((tabPosition == Qt.TopEdge)? parent.height -1 : parent.height )
                        implicitWidth: 1
                        color: palette.darkShade
                    }
                    Rectangle { //Highlight on the left side of a tab
                        id: leftTabHighlight
                        anchors.left: parent.left
                        anchors.top: (tabPosition == Qt.TopEdge)? topTabHighlight.bottom : topTabHighlight.top
                        implicitHeight: styleData.selected ? parent.height -1 : ((tabPosition == Qt.TopEdge) ? parent.height -2 : parent.height)
                        implicitWidth: 1
                        color: styleData.selected ? palette.lightestShade : palette.lighterShade
                    }
                    Rectangle { //Active focus highlight
                        anchors.bottom: parent.bottom
                        height: parent.height - defaultSpacing.doubleBorderSize + 1
                        width: parent.width - defaultSpacing.doubleBorderSize
                        color : "transparent"
                        border.width: 1
                        border.color: styleData.activeFocus ? palette.lighterShade : "transparent"
                    }
                }
            }
    }

    leftCorner: Rectangle { //Area spacer on the left of tabs
        color:"transparent" //covered by tabBar
        implicitWidth: defaultSpacing.standardMargin
        implicitHeight: tabHeight
    }

    tabBar: Rectangle {
        color: palette.mainWindowColor
        Rectangle { //highlight line at bottom of tabbar
            id: bottomTabBarHighlight
            anchors.bottom: parent.bottom
            implicitHeight: 1
            implicitWidth: parent.width
            color: (tabPosition == Qt.BottomEdge)?( "transparent") : palette.lightestShade
        }
    }

    //TODO: Figure out a way to include a border/margin within the tab frame
    //Current workflow requires every use of tabview to have its own border
    //Ideally the border would be contained within here. All attempts have failed
    frame: Rectangle {
            color: palette.midLightColor
        }
}
