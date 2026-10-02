import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import EmulatorPSW7 1.0

ApplicationWindow {
    visible: true

    width: 1100
    height: 700

    title: "Emulator PSW7"

    TcpClient {
        id: client

        onConnectedChanged: function(connected) {
            statusLabel.text = connected
                ? "Connected"
                : "Disconnected"
        }

        onResponseReceived: function(response) {
            responseArea.text = response
            historyModel.reload()
        }

        onErrorOccurred: function(error) {
            responseArea.text = "Error: " + error
        }
    }

    HistoryModel {
        id: historyModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 15

        Label {
            text: "Emulator PSW7"
            font.pixelSize: 30
            font.bold: true
            color: "#ffffff"
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: hostField

                Layout.fillWidth: true

                text: "127.0.0.1"
                placeholderText: "Host"
            }

            TextField {
                id: portField

                Layout.preferredWidth: 100

                text: "5025"
                placeholderText: "Port"
            }

            Button {
                text: "Connect"

                onClicked: {
                    client.connectToServer(
                        hostField.text,
                        Number(portField.text)
                    )
                }
            }

            Label {
                id: statusLabel

                text: "Disconnected"
                color: "#ffffff"
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: commandField

                Layout.fillWidth: true

                text: "*IDN?"
                placeholderText: "SCPI command"

                onAccepted: {
                    client.sendCommand(
                        commandField.text
                    )
                }
            }

            Button {
                text: "Send"

                onClicked: {
                    client.sendCommand(
                        commandField.text
                    )
                }
            }
        }

        Label {
            text: "Response"
            font.bold: true
            color: "#ffffff"
        }

        TextArea {
            id: responseArea

            Layout.fillWidth: true
            Layout.preferredHeight: 100

            readOnly: true
            wrapMode: TextArea.Wrap

            placeholderText: "Device response..."
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Command history"
                font.bold: true
                font.pixelSize: 18
                color: "#ffffff"

                Layout.fillWidth: true
            }

            Button {
                text: "Refresh"

                onClicked: {
                    historyModel.reload()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: "#202020"

            border.width: 1
            border.color: "#404040"

            ListView {
                anchors.fill: parent
                anchors.margins: 1

                model: historyModel

                clip: true

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 60

                    color: index % 2 === 0
                        ? "#292929"
                        : "#232323"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Label {
                            Layout.preferredWidth: 150

                            text: timestamp
                            color: "#dddddd"

                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.preferredWidth: 80

                            text: device
                            color: "#dddddd"
                        }

                        Label {
                            Layout.preferredWidth: 180

                            text: command
                            color: "#ffffff"
                            font.bold: true

                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true

                            text: response
                            color: "#cccccc"

                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}