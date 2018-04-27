/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import WGControls 1.0

/*!
  \ingroup wgcontrols
  \brief Style file for WGScrollView

*/
ScrollViewStyle {
    WGComponent { type: "WGScrollViewStyle20" }

    transientScrollBars: false

    /* Margins can be added by setting padding property eg:

    padding {left: 5; top: 5; right: 5; bottom: 5} */

    property int expandedSize: defaultSpacing.scrollBarSize * 2

    corner: Item {}

    frame: Item {}

    scrollBarBackground: Item {
        property bool sticky: false
        property bool hovered: styleData.hovered

        implicitWidth: expandedSize
        implicitHeight: expandedSize

        clip: true
        opacity: transientScrollBars ? 0.5 : 1.0
        visible: !Settings.hasTouchScreen && (!transientScrollBars || sticky)
        Rectangle {
            color: palette.lighterShade

            border.width: defaultSpacing.standardBorderSize
            border.color: palette.darkestShade
            anchors.fill: parent

            anchors.rightMargin: styleData.horizontal ? -2 : -1
            anchors.leftMargin: styleData.horizontal ? -2 : 0
            anchors.topMargin: styleData.horizontal ? 0 : -2
            anchors.bottomMargin: styleData.horizontal ? -1 : -2
        }
        onHoveredChanged: if (hovered) sticky = true
        onVisibleChanged: if (!visible) sticky = false
    }

    handle: Rectangle {
        property bool sticky: false
        property bool hovered: __activeControl !== "none"
        implicitWidth: expandedSize + 1
        implicitHeight: expandedSize + 1

        color: palette.highlightColor

        border.width: defaultSpacing.standardBorderSize
        border.color: palette.darkHeaderColor

        onHoveredChanged: if (hovered) sticky = true
        onVisibleChanged: if (!visible) sticky = false
    }

    incrementControl: Item {
        visible: !transientScrollBars
        implicitWidth: transientScrollBars ? 0 : expandedSize
        implicitHeight: transientScrollBars ? 0 : expandedSize
        WGButtonFrame {
            id: buttonFrame
            anchors.fill: parent
            anchors.bottomMargin: -1
            anchors.rightMargin: -1

            radius: defaultSpacing.halfRadius

            Text {
                id: arrowTextInc
                color : control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                anchors.centerIn: parent

                anchors.verticalCenterOffset: styleData.horizontal ? 1 : 0

                font.family : "Marlett"
                font.pixelSize: Math.round(parent.height)

                renderType: Text.QtRendering
                text : styleData.horizontal ? "\uF034" : "\uF036"
            }

            states: [
                State {
                    name: "PRESSED"
                    when: styleData.pressed && control.enabled
                    PropertyChanges {target: buttonFrame; color: palette.darkShade}
                    PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
                },
                State {
                    name: "HOVERED"
                    when: styleData.hovered && control.enabled
                    PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                },
                State {
                    name: "DISABLED"
                    when: !control.enabled
                    PropertyChanges {target: buttonFrame; color: "transparent"}
                    PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                    PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
                }
            ]
        }
    }

    decrementControl: Item {
        visible: !transientScrollBars
        implicitWidth: transientScrollBars ? 0 : expandedSize
        implicitHeight: transientScrollBars ? 0 : expandedSize
        WGButtonFrame {
            id: horizButtonFrame
            anchors.fill: parent
            anchors.topMargin: styleData.horizontal ? 0 : -1
            anchors.leftMargin:  styleData.horizontal ? -1 : 0
            anchors.bottomMargin: styleData.horizontal ? -1 : 0
            anchors.rightMargin: styleData.horizontal ? 0 : -1

            radius: defaultSpacing.halfRadius

            Text {
                id: arrowTextDec
                color : control.enabled ? palette.neutralTextColor : palette.disabledTextColor

                anchors.centerIn: parent

                anchors.verticalCenterOffset: styleData.horizontal ? 1 : 0

                font.family : "Marlett"
                font.pixelSize: Math.round(parent.height)

                renderType: Text.QtRendering
                text : styleData.horizontal ? "\uF033" : "\uF035"
            }

            states: [
                State {
                    name: "PRESSED"
                    when: styleData.pressed && control.enabled
                    PropertyChanges {target: horizButtonFrame; color: palette.darkShade}
                    PropertyChanges {target: horizButtonFrame; innerBorderColor: "transparent"}
                },
                State {
                    name: "HOVERED"
                    when: styleData.hovered && control.enabled
                    PropertyChanges {target: horizButtonFrame; highlightColor: palette.lighterShade}
                },
                State {
                    name: "DISABLED"
                    when: !control.enabled
                    PropertyChanges {target: horizButtonFrame; color: "transparent"}
                    PropertyChanges {target: horizButtonFrame; borderColor: palette.darkShade}
                    PropertyChanges {target: horizButtonFrame; innerBorderColor: "transparent"}
                }
            ]
        }
    }
}
