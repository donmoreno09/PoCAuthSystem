import QtQuick 6.8
import QtQuick.Controls 6.8
import QtQuick.Layouts 6.8

import PoCAuthSystem 1.0

ApplicationWindow {
    id: app
    width: 800
    height: 600
    visible: true
    title: qsTr("PoC Auth System")

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16
        visible: AuthManager.state === AuthStateEnum.Authenticated

        Text {
            text: "Welcome, " + AuthManager.displayName + "!"
            font.pointSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Username: " + AuthManager.username
            font.pointSize: 12
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Email: " + AuthManager.email
            font.pointSize: 12
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Roles: " + AuthManager.roles.join(", ")
            font.pointSize: 12
            color: "#666"
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.preferredHeight: 8 }

        Button {
            text: qsTr("Logout")
            Layout.alignment: Qt.AlignHCenter
            onClicked: AuthManager.logout()
        }
    }

    // Auth guard
    Loader {
        anchors.fill: parent
        z: 100
        active: AuthManager.state !== AuthStateEnum.Authenticated

        sourceComponent: LoginPage {}
    }
}
