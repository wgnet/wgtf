import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

import WGControls 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0

Rectangle {
    color: palette.mainWindowColor

    WGTreeModel {
        id: qmlPropertyTreeModel
        WGModelRow {
            WGModelData {
                roles: {"display": "Int", "enabled": true, "valueType": "int", "value": 5}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "Float", "enabled": true, "valueType": "float", "decimals": 2, "stepSize": 0.01, "value": 5.5}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "Color3", "enabled": true, "readOnly": false, "isColor": 1, "valueType": "vector3", "value": Qt.vector3d(100, 2, 3)}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "String", "enabled": true, "value": "'Alice is lying to Bob and Eve'", "valueType": "string"}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "File", "isUrl": 1, "value": ""}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "HDRColor", "enabled": true, "readOnly": false, "HDRColor": 1, "valueType": "vector4", "value": Qt.vector4d(100, 2, 3, 95)}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "Slider", "enabled": true, "isSlider": 1, "minValue": 3, "maxValue": 57, "stepSize": 0.5, "decimals": 1, "value": 7}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "Thumbnail", "enabled": true, "isThumbnail": 1, "image": "file:///C:/path_to.jpg"}
            }
        }
        WGModelRow {
            WGModelData {
                roles: {"display": "Time", "enabled": true, "valueType": "time", "minValue": 3, "maxValue": 57}
            }
        }
        WGModelRow {
            property var testList: ListModel {
                id: testPolyList
                ListElement {
                    value: "alice"
                    display: "Alice"
                }
                ListElement {
                    value: "bob"
                    display: "Bob"
                }
                ListElement {
                    value: "eve"
                    display: "Eve"
                }
            }
            WGModelData {
                roles: {"display": "Polystruct", "enabled": true, "valueType": "polystruct", "definitionModel": testPolyList}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "Angle", "enabled": true, "readOnly": false, "valueType": "angle", "value": 3, "decimals": 1, "stepSize": 0.1, "minValue": 0, "maxValue": 360}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "vector4", "enabled": true, "valueType": "vector4", "value": Qt.vector4d(1, 2, 3, 4)}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "Vector3", "enabled": true, "valueType": "vector3", "value": Qt.vector3d(1, 2, 3)}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "Vector2", "enabled": true, "valueType": "vector2", "value": Qt.vector2d(1, 2)}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "Bool", "enabled": true, "valueType": "bool", "value": true}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "String", "enabled": true, "value": "'Alice is lying to Bob and Eve'", "valueType": "string"}
            }
        }

        WGModelRow {
            WGModelData {
                roles: {"display": "Vector types"}
            }
            children: [
                WGModelRow {
                    WGModelData {
                        roles: {"display": "vector3", "enabled": true, "valueType": "vector3", "value": Qt.vector3d(1, 2, 3)}
                    }
                },
                WGModelRow {
                    WGModelData {
                        roles: {"display": "vector4", "enabled": true, "valueType": "vector4", "value": Qt.vector4d(1, 2, 3, 4)}
                    }
                }
            ]
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
            text: "Example WGPropertyTreeView using a QML WGTreeModel"
            color: palette.textColor
        }

        ScrollView{
            frameVisible: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            WGPropertyTreeView {
                id: propertytree
                model: qmlPropertyTreeModel
            }
        }
    }
}
