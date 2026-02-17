import QtQuick 6.8
import QtQuick.Controls 6.8
import QtQuick.Layouts 6.8

import PoCAuthSystem 1.0

ApplicationWindow {
    id: app
    width: 900
    height: 700
    visible: true
    title: qsTr("PoC Auth System")

    color: "black"

    Flickable {
        anchors.fill: parent
        anchors.margins: 20
        contentHeight: mainCol.implicitHeight
        clip: true
        visible: AuthManager.state === AuthStateEnum.Authenticated

        ColumnLayout {
            id: mainCol
            width: parent.width
            spacing: 20

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                ColumnLayout {
                    spacing: 2

                    Text {
                        text: "Welcome, " + AuthManager.displayName + "!"
                        font.pointSize: 18
                        font.bold: true
                        color: "#e0e0e0"
                    }
                    Text {
                        text: AuthManager.username + " / " + AuthManager.email
                        font.pointSize: 10
                        color: "#888"
                    }
                    Text {
                        text: "Roles: " + AuthManager.roles.join(", ")
                        font.pointSize: 10
                        color: "#888"
                    }
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("Logout")
                    onClicked: AuthManager.logout()
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

            Text {
                text: "Permissions"
                font.pointSize: 14
                font.bold: true
                color: "#e0e0e0"
            }

            GridLayout {
                Layout.fillWidth: true
                columns: Math.max(1, Math.floor((app.width - 40) / 170))
                columnSpacing: 8
                rowSpacing: 8

                Repeater {
                    model: ListModel {
                        ListElement { category: "Notifications"; perm: "notifications.read" }
                        ListElement { category: "Alert Zones";   perm: "alertzone.create" }
                        ListElement { category: "Alert Zones";   perm: "alertzone.delete" }
                        ListElement { category: "POI";           perm: "poi.create" }
                        ListElement { category: "POI";           perm: "poi.delete" }
                        ListElement { category: "AIS";           perm: "ais.show" }
                        ListElement { category: "Doc Space";     perm: "doc-space.show" }
                        ListElement { category: "Trucks";        perm: "truck.show" }
                        ListElement { category: "Items";         perm: "items.read" }
                        ListElement { category: "Items";         perm: "items.write" }
                        ListElement { category: "Items";         perm: "items.delete" }
                    }

                    delegate: Rectangle {
                        required property string category
                        required property string perm

                        property bool granted: PermissionManager.revision >= 0 && PermissionManager.hasPermission(perm)

                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 48
                        radius: 6
                        color: granted ? "#1b4332" : "#3d1f1f"
                        border.color: granted ? "#2ecc71" : "#e74c3c"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 2

                            Text {
                                text: category
                                font.pointSize: 8
                                font.bold: true
                                color: granted ? "#2ecc71" : "#e74c3c"
                            }
                            Text {
                                text: perm
                                font.pointSize: 9
                                color: "#ccc"
                            }
                        }
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

            // ── Items CRUD ──────────────────────────────────────────────
            Text {
                text: "Items"
                font.pointSize: 14
                font.bold: true
                color: "#e0e0e0"
            }

            // Error banner
            Rectangle {
                Layout.fillWidth: true
                height: 32
                radius: 6
                color: "#3d1f1f"
                border.color: "#e74c3c"
                border.width: 1
                visible: ItemModel.error !== ""

                Text {
                    anchors.centerIn: parent
                    text: ItemModel.error
                    color: "#e74c3c"
                    font.pointSize: 9
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: createRow.implicitHeight + 16
                radius: 6
                color: "#1a1a2e"
                border.color: "#444"
                border.width: 1
                visible: PermissionManager.revision >= 0 && PermissionManager.hasPermission("items.write")

                RowLayout {
                    id: createRow
                    anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
                    anchors.margins: 8
                    spacing: 8

                    TextField {
                        id: newName
                        placeholderText: "Name"
                        Layout.fillWidth: true
                        color: "#e0e0e0"
                    }

                    ComboBox {
                        id: newStatus
                        model: ["active", "inactive", "maintenance"]
                        implicitWidth: 130
                    }

                    Button {
                        text: ItemModel.loading ? "…" : "Create"
                        enabled: !ItemModel.loading && newName.text.trim() !== ""
                        onClicked: {
                            ItemModel.create(newName.text.trim(), newStatus.currentText)
                            newName.text = ""
                        }
                    }
                }
            }

            ListView {
                Layout.fillWidth: true
                implicitHeight: contentHeight
                clip: true
                model: ItemModel
                spacing: 6

                delegate: Rectangle {
                    required property string id
                    required property string name
                    required property string status
                    required property int index

                    width: ListView.view.width
                    height: itemRow.implicitHeight + 12
                    radius: 6
                    color: "#16213e"
                    border.color: "#333"
                    border.width: 1

                    RowLayout {
                        id: itemRow
                        anchors { left: parent.left; right: parent.right; verticalCenter: parent.verticalCenter }
                        anchors.margins: 10
                        spacing: 8

                        Text {
                            text: name
                            color: "#e0e0e0"
                            font.pointSize: 10
                            Layout.fillWidth: true
                        }

                        Text {
                            text: status
                            color: "#888"
                            font.pointSize: 9
                        }

                        RowLayout {
                            spacing: 4
                            visible: PermissionManager.revision >= 0 && PermissionManager.hasPermission("items.write")

                            TextField {
                                id: editName
                                text: name
                                implicitWidth: 120
                                color: "#e0e0e0"
                                font.pointSize: 9
                            }

                            ComboBox {
                                id: editStatus
                                model: ["active", "inactive", "maintenance"]
                                currentIndex: model.indexOf(status)
                                implicitWidth: 120
                                font.pointSize: 9
                            }

                            Button {
                                text: "Save"
                                enabled: !ItemModel.loading
                                onClicked: ItemModel.update(id, editName.text.trim(), editStatus.currentText)
                            }
                        }

                        Button {
                            text: "DELETE"
                            visible: PermissionManager.revision >= 0 && PermissionManager.hasPermission("items.delete")
                            enabled: !ItemModel.loading
                            onClicked: ItemModel.remove(id)
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: ItemModel.loading ? "Loading…" : "No items"
                    color: "#555"
                    font.pointSize: 10
                    visible: ItemModel.count === 0
                }
            }

            Item { Layout.preferredHeight: 20 }
        }
    }

    Loader {
        anchors.fill: parent
        z: 100
        active: AuthManager.state !== AuthStateEnum.Authenticated
        sourceComponent: LoginPage {}
    }
}
