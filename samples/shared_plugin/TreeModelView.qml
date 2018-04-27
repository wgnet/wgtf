import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Styles 2.0
import WGControls.Views 2.0

Rectangle {
    color: palette.mainWindowColor

    WGTreeModel {
        id: qmlWGTreeModel
        header: WGModelRow {
            WGModelData {
                roles: {"headerText": "h1", "footerText": "f1"}
            }
            WGModelData {
                roles: {"headerText": "h2", "footerText": "f2"}
            }
            WGModelData {
                roles: {"headerText": "h3", "footerText": "f3"}
            }
            WGModelData {
                roles: {"headerText": "h4", "footerText": "f4"}
            }
            WGModelData {
                roles: {"headerText": "h5", "footerText": "f5"}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "This"}
            }
            WGModelData {
                roles: {"display": "is"}
            }
            WGModelData {
                roles: {"display": "a"}
            }
            WGModelData {
                roles: {"display": "QML"}
            }
            WGModelData {
                roles: {"display": "model"}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "This"}
            }
            WGModelData {
                roles: {"display": "is"}
            }
            WGModelData {
                roles: {"display": "a"}
            }
            WGModelData {
                roles: {"display": "QML"}
            }
            WGModelData {
                roles: {"display": "model"}
            }
            children: [
                WGModelRow {
                    WGModelData {
                        roles: {"display": "This"}
                    }
                    WGModelData {
                        roles: {"display": "is"}
                    }
                    WGModelData {
                        roles: {"display": "a"}
                    }
                    WGModelData {
                        roles: {"display": "child"}
                    }
                    WGModelData {
                        roles: {"display": "row"}
                    }
                    children: [
                        WGModelRow {
                            WGModelData {
                                roles: {"display": "This"}
                            }
                            WGModelData {
                                roles: {"display": "is"}
                            }
                            WGModelData {
                                roles: {"display": "a"}
                            }
                            WGModelData {
                                roles: {"display": "grandchild"}
                            }
                            WGModelData {
                                roles: {"display": "row"}
                            }
                        }]
                },
                WGModelRow {
                    WGModelData {
                        roles: {"display": "This"}
                    }
                    WGModelData {
                        roles: {"display": "is"}
                    }
                    WGModelData {
                        roles: {"display": "another"}
                    }
                    WGModelData {
                        roles: {"display": "child"}
                    }
                    WGModelData {
                        roles: {"display": "row"}
                    }
                }]
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: defaultSpacing.standardMargin

        Item {
            id:spacerItem
            height: defaultSpacing.standardMargin
        }

        Text {
            id: titleText
            text: "Example WGTreeView using a QML WGTreeModel"
            color: palette.textColor
        }

        ScrollView{
            frameVisible: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGTreeView {
                id: exampleWGTreeView
                model: qmlWGTreeModel
            }
        }
    }
}

