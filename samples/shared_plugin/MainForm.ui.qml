import QtQuick 2.6
import WGControls 2.0
import QtQuick.Controls 1.4

TabView {
    Tab {
        title: "WGListView"
        ListModelView {
        }
    }
    Tab {
        title: "WGTableView"
        TableModelView {
        }
    }
    Tab {
        title: "WGTreeModelView"
        TreeModelView {
        }
    }
    Tab {
        title: "WGPropertyModelView"
        PropertyModelTreeView {
            anchors.fill: parent
        }
    }
    Tab {
        title: "WGControls Test"
        WGControlsTest {
        }

    }
    Tab {
        title: "Scratch Pad"
        ScratchPad {
        }
    }
}


