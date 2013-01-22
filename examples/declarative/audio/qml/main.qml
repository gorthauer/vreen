import QtQuick 1.1
import com.vk.api 1.0

Rectangle {
    id: root

    width: 500
    height: 800

    TextEdit {
        id: login

        onLinkActivated: client.connectToHost()

        z: 10
        anchors.centerIn: parent
        text: qsTr("<a href=\"http://vk.com\">Click to login</a>")
        textFormat: TextEdit.RichText
    }

    AudioModel {
        id: audioModel
        client: client
    }

    Client {
        id: client

        onOnlineChanged: {
            if (online) {
                audioModel.getAudio();
            }
        }

        OAuthConnection {
            id: conn

            Component.onCompleted: {
                setConnectionOption(Connection.ShowAuthDialog, true);
                setConnectionOption(Connection.KeepAuthData, true);
            }

            clientId: 3220807
            displayType: OAuthConnection.Popup
        }
    }

    ListView {
        id: audioView

        anchors.fill: parent
        scale: 0
        model: audioModel

        header: Text {
            width: parent.width
            height: 50
            text: qsTr("%1 latest audio").arg(client.me ? client.me.name : qsTr("Unknown"))
            font.bold: true
            font.pixelSize: login.font.pixelSize * 1.1
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
        }

        delegate: Rectangle {
            id: item

            width: parent.width
            height: 90
            color: index % 2 ? syspal.alternateBase : "transparent"

            Image {
                id: preview

                source: "images/media-optical-audio.png"

                fillMode: Image.PreserveAspectFit
                clip: true
                smooth: true

                anchors {
                    left: parent.left
                    leftMargin: 5
                    top: column.top
                }
            }

            Column {
                id: column

                spacing: 2

                anchors {
                    left: preview.right
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                    leftMargin: 10
                    rightMargin: 10
                    topMargin: 10
                }

                Text {
                    id: titleLabel
                    width: parent.width
                    font.bold: true
                    text: artist
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 1
                }
                Text {
                    id: descriptionLabel
                    width: parent.width
                    text: title
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                }
                Text {
                    id: urlLabel
                    width: parent.width
                    text: url
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 1
                }
            }

            Text {
                id: dateLabel

                color: syspal.dark
                font.pixelSize: login.font.pixelSize * 0.8

                anchors {
                    bottom: hr.top
                    bottomMargin: 3
                    left: column.left
                }
                text: qsTr("duration %2").arg(duration)
            }

            Rectangle {
                id: hr
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color: syspal.window
            }
        }

        ScrollDecorator {
            flickableItem: parent
        }
    }

    SystemPalette {
        id: syspal
    }

    states: [
        State {
            name: "online"
            when: client.online
            PropertyChanges {
                target: login
                scale: 0
            }
            PropertyChanges {
                target: audioView
                scale: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "online"
            NumberAnimation { target: audioView; property: "scale"; duration: 400; easing.type: Easing.InOutQuad }
            NumberAnimation { target: login; property: "scale"; duration: 400; easing.type: Easing.InOutQuad }
        }
    ]
}
