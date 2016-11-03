import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

import WGControls 2.0
import WGControls.Layouts 2.0

/*!
    \brief Hello World example.
    Please write Doxygen comments like this.
*/
WGPanel {

    // All elements should have an id
    id: helloPanel

    /*! Title of panel.
        Document new properties which are added with the "property" keyword.
    */
    property var title: qsTr( "Hello World" )

    color: palette.MainWindowColor

    property var source: sourceModel

    WGExpandingRowLayout {
        id: layoutHello

        anchors.fill: parent

        Column
        {
            WGLabel {
                id: labelHello
                objectName: "ClickCount"
                text: "Clicks " + source.clickCount
            }

            WGLabel {
                id: labelClickDescription
                objectName: "ClickDescription"
                text: source.clickDescription
            }
        }

        WGPushButton {
            id: buttonHello
            text: "Click Here"
            iconSource: "icons/icon_320x200.png"
            Layout.fillWidth: true
            Layout.fillHeight: true
            onClicked: {
                source.clickButton(1);
            }
        }
    }

    Connections {
        target: source

        /*! Gets called when clickButton or undo/redo is called
            Notifies anything using source.clickCount to update
        */
        onClickButtonInvoked: {
            source.clickCountChanged(0);
        }
    }

    Connections {
        target: source

        /*! Gets called when clickButton or undo/redo is called
            Notifies anything using source.clickDescription to update
        */
        onClickButtonInvoked: {
            source.clickDescriptionChanged(0);
        }
    }
}
