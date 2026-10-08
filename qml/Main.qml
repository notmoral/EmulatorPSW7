import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import EmulatorPSW7 1.0

ApplicationWindow {
    visible: true
    width: 1100
    height: 700
    title: "Emulator PSW7"
    color: "#1e1e1e"

    property color bgDark: "#1e1e1e"
    property color bgMedium: "#2d2d2d"
    property color bgLight: "#3c3c3c"
    property color borderColor: "#555555"
    property color textPrimary: "#ffffff"
    property color textSecondary: "#aaaaaa"
    property color accentBlue: "#0e639c"
    property color accentHover: "#1177bb"

    // Активный клиент — TCP или UDP, в зависимости от выбора
    property var activeClient: protocolSelector.currentIndex === 0 ? tcpClient : udpClient

    TcpClient {
        id: tcpClient

        onConnectedChanged: function(connected) {
            if (protocolSelector.currentIndex === 0) {
                statusLabel.text = connected ? "Connected" : "Disconnected"
                statusLabel.color = connected ? "#88dd88" : "#dd8888"
            }
        }
        onResponseReceived: function(response) {
            if (protocolSelector.currentIndex === 0) {
                responseArea.text = response
                historyModel.reload()
            }
        }
        onErrorOccurred: function(error) {
            if (protocolSelector.currentIndex === 0) {
                responseArea.text = "Error: " + error
            }
        }
    }

    UdpClient {
        id: udpClient

        onConnectedChanged: function(connected) {
            if (protocolSelector.currentIndex === 1) {
                statusLabel.text = connected ? "Connected" : "Disconnected"
                statusLabel.color = connected ? "#88dd88" : "#dd8888"
            }
        }
        onResponseReceived: function(response) {
            if (protocolSelector.currentIndex === 1) {
                responseArea.text = response
                historyModel.reload()
            }
        }
        onErrorOccurred: function(error) {
            if (protocolSelector.currentIndex === 1) {
                responseArea.text = "Error: " + error
            }
        }
    }

    HistoryModel {
        id: historyModel
    }

    CommandImporter {
        id: importer

        onImportStarted: function(total) {
            importStatus.text = "Импорт: 0/" + total
            importStatus.color = "#88dd88"
        }
        onImportProgress: function(current, total) {
            importStatus.text = "Импорт: " + current + "/" + total
        }
        onImportFinished: function(total) {
            importStatus.text = "Импорт завершён: " + total + " команд"
            importStatus.color = "#88dd88"
            historyModel.reload()
        }
        onImportError: function(error) {
            importStatus.text = "Ошибка: " + error
            importStatus.color = "#dd8888"
        }
    }

    FileDialog {
        id: fileDialog
        title: "Файл с командами"
        nameFilters: ["Text files (*.txt)", "All files (*)"]

        onAccepted: {
            let path = selectedFile.toString()
            path = decodeURIComponent(path.replace(/^file:\/{2,3}/, ""))

            if (importer.loadFile(path)) {
                importer.startImport(
                    hostField.text,
                    Number(portField.text)
                )
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 15

        Label {
            text: "Emulator PSW7"
            font.pixelSize: 30
            font.bold: true
            color: textPrimary
        }

        // Строка подключения
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: hostField
                Layout.fillWidth: true
                text: "127.0.0.1"
                placeholderText: "Host"
                color: textPrimary
                placeholderTextColor: textSecondary
                background: Rectangle {
                    color: bgLight
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
            }

            TextField {
                id: portField
                Layout.preferredWidth: 100
                text: "5025"
                placeholderText: "Port"
                color: textPrimary
                placeholderTextColor: textSecondary
                background: Rectangle {
                    color: bgLight
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
            }

            ComboBox {
                id: protocolSelector
                Layout.preferredWidth: 90
                model: ["TCP", "UDP"]
                currentIndex: 0

                onActivated: {
                    // При смене протокола отключаем оба клиента
                    if (tcpClient.connected) tcpClient.disconnectFromServer()
                    if (udpClient.connected) udpClient.disconnectFromServer()
                    statusLabel.text = "Disconnected"
                    statusLabel.color = "#dd8888"
                }
            }

            Button {
                text: activeClient.connected ? "Disconnect" : "Connect"
                onClicked: {
                    if (activeClient.connected) {
                        activeClient.disconnectFromServer()
                    } else {
                        activeClient.connectToServer(
                            hostField.text,
                            Number(portField.text)
                        )
                    }
                }

                background: Rectangle {
                    color: parent.pressed
                        ? accentHover
                        : (parent.hovered ? "#4a4a4a" : bgLight)
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: "Import commands"
                enabled: !importer.running
                onClicked: fileDialog.open()

                background: Rectangle {
                    color: parent.pressed
                        ? accentHover
                        : (parent.hovered ? "#4a4a4a" : bgLight)
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: parent.enabled ? textPrimary : textSecondary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Label {
                id: statusLabel
                text: "Disconnected"
                color: "#dd8888"
                font.bold: true
            }
        }

        // Строка ввода SCPI-команд
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: commandField
                Layout.fillWidth: true
                text: "*IDN?"
                placeholderText: "SCPI command"
                color: textPrimary
                placeholderTextColor: textSecondary
                background: Rectangle {
                    color: bgLight
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }

                onAccepted: activeClient.sendCommand(commandField.text)
            }

            Button {
                text: "Send"
                onClicked: activeClient.sendCommand(commandField.text)

                background: Rectangle {
                    color: parent.pressed
                        ? accentHover
                        : (parent.hovered ? "#4a4a4a" : bgLight)
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Label {
            id: importStatus
            text: ""
            color: textSecondary
            font.italic: true
        }

        Label {
            text: "Response"
            font.bold: true
            color: textPrimary
        }

        TextArea {
            id: responseArea
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            readOnly: true
            wrapMode: TextArea.Wrap
            color: textPrimary
            placeholderText: "Device response..."
            placeholderTextColor: textSecondary

            background: Rectangle {
                color: bgMedium
                radius: 4
                border.color: borderColor
                border.width: 1
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "Command history"
                font.bold: true
                font.pixelSize: 18
                color: textPrimary
                Layout.fillWidth: true
            }

            Button {
                text: "Refresh"
                onClicked: historyModel.reload()

                background: Rectangle {
                    color: parent.pressed
                        ? accentHover
                        : (parent.hovered ? "#4a4a4a" : bgLight)
                    radius: 4
                    border.color: borderColor
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: bgMedium
            border.width: 1
            border.color: borderColor
            radius: 4

            ListView {
                anchors.fill: parent
                anchors.margins: 1
                model: historyModel
                clip: true

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 60
                    color: index % 2 === 0 ? bgMedium : "#252525"
                    border.color: "#333333"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Label {
                            Layout.preferredWidth: 150
                            text: timestamp
                            color: textSecondary
                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.preferredWidth: 80
                            text: device
                            color: textSecondary
                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.preferredWidth: 180
                            text: command
                            color: textPrimary
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
