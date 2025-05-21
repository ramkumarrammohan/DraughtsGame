import QtQuick 2.15

import com.checkerboard.Theme 1.0
import com.checkerboard.Enums 1.0

Rectangle {
    color: Theme.green
    width: parent.width
    height: 50 * Theme.scaleH
    radius: 10 * Theme.scaleW

    Row {
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            width: 20 * Theme.scaleW
            height: width
            radius: width / 2
            color: board ? (board.activePlayer === Enums.PlayerA ? Theme.coinLight
                                                         : Theme.coinDark)
                         : Theme.coinDark
            anchors.verticalCenter: parent.verticalCenter
        }

        CTextBold {
            text: qsTr("Active Player")
            color: Theme.white
            font.pixelSize: 18 * Theme.scaleFont
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Image {
        width: height
        height: parent.height - (10 * Theme.scaleH)
        source: "qrc:/Qml/Icon/reset.png"
        anchors {
            right: parent.right
            rightMargin: (10 * Theme.scaleW)
            verticalCenter: parent.verticalCenter
        }

        MouseArea {
            anchors.fill: parent
            onClicked: board.reset()
        }
    }

    Rectangle {
        id: messageItemId
        color: Theme.red
        width: parent.width
        height: 50 * Theme.scaleH
        radius: 10 * Theme.scaleW
        visible: timerId.running
        CTextBold {
            id: messageTextId
            color: Theme.white
            font.pixelSize: 18 * Theme.scaleFont
            anchors.fill: parent
        }
        Timer {
            id: timerId
            interval: 3000
        }
        function show()
        {
            timerId.start()
        }
    }

    Connections {
        target: board
        function onInvalidMove(msg)
        {
            messageTextId.text = msg;
            messageItemId.show()
        }
    }
}
