import QtQuick 6.8
import QtQuick.Controls 6.8
import QtQuick.Layouts 6.8

import PoCAuthSystem 1.0

Rectangle {
    id: root
    color: "#1a1a2e"

    readonly property bool isBusy: AuthManager.state === AuthStateEnum.LoggingIn
                                   || AuthManager.state === AuthStateEnum.AutoLoggingIn
    readonly property bool hasError: AuthManager.state === AuthStateEnum.Error

    ColumnLayout {
        anchors.centerIn: parent
        width: 360
        spacing: 16

        // Title
        Text {
            text: "PoC Auth System"
            color: "#e0e0e0"
            font {
                pointSize: 24
                weight: Font.Bold
            }
            Layout.alignment: Qt.AlignHCenter
        }

        // Subtitle
        Text {
            text: qsTr("Authentication Proof of Concept")
            color: "#888"
            font.pointSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.preferredHeight: 12 }

        // Auto-login message
        Text {
            text: qsTr("Restoring session...")
            color: "#888"
            font.pointSize: 11
            visible: AuthManager.state === AuthStateEnum.AutoLoggingIn
            Layout.alignment: Qt.AlignHCenter
        }

        // Username
        ColumnLayout {
            spacing: 4
            Layout.fillWidth: true
            visible: AuthManager.state !== AuthStateEnum.AutoLoggingIn

            Label {
                text: qsTr("Username")
                color: "#ccc"
                font.pointSize: 10
            }

            TextField {
                id: usernameInput
                placeholderText: qsTr("Enter username")
                Layout.fillWidth: true
                enabled: !root.isBusy
            }
        }

        // Password
        ColumnLayout {
            spacing: 4
            Layout.fillWidth: true
            visible: AuthManager.state !== AuthStateEnum.AutoLoggingIn

            Label {
                text: qsTr("Password")
                color: "#ccc"
                font.pointSize: 10
            }

            TextField {
                id: passwordInput
                placeholderText: qsTr("Enter password")
                echoMode: TextInput.Password
                Layout.fillWidth: true
                enabled: !root.isBusy

                Keys.onReturnPressed: {
                    if (loginButton.enabled) loginButton.clicked()
                }
            }
        }

        // Error message
        Text {
            text: AuthManager.errorMessage
            color: "#e74c3c"
            font.pointSize: 10
            visible: root.hasError && AuthManager.errorMessage !== ""
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        // Login button
        Button {
            id: loginButton
            text: AuthManager.state === AuthStateEnum.LoggingIn
                  ? qsTr("Logging in...") : qsTr("Login")
            Layout.fillWidth: true
            visible: AuthManager.state !== AuthStateEnum.AutoLoggingIn
            enabled: !root.isBusy
                     && usernameInput.text.length > 0
                     && passwordInput.text.length > 0
            onClicked: AuthManager.login(usernameInput.text, passwordInput.text)
        }
    }

    // Clear fields on successful login
    Connections {
        target: AuthManager
        function onLoginSucceeded() {
            usernameInput.text = ""
            passwordInput.text = ""
        }
    }
}
