import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import com.checkerboard.Theme 1.0
import com.checkerboard.Enums 1.0

Item {
    id: coinRootId
    width: 50 * Theme.scaleW
    height: width

    property var item: null
    property int row: -1
    property int col: -1
    property bool dark: false
    property color color: dark ? Theme.coinDark : Theme.coinLight

    Component.onCompleted: {
        updateState()
    }

    // Shadow Effect
    DropShadow {
        anchors.fill: draughtsPiece
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8
        samples: 16
        color: "#80000000" // Semi-transparent black
        source: draughtsPiece
    }

    Rectangle {
        id: draughtsPiece
        color: coinRootId.color
        anchors.fill: parent
        border.color: (coinRootId.dark ? Qt.lighter(coinRootId.color, 1.25)
                                       : Qt.darker(coinRootId.color, 1.25))
        border.width: 2 * Theme.scaleW
        radius: width / 2

        Rectangle {
            id: draughtsPieceInner
            color: coinRootId.color
            anchors.fill: parent
            anchors.margins: coinRootId.width * 0.1
            border.color: (coinRootId.dark ? Qt.lighter(coinRootId.color, 1.25)
                                           : Qt.darker(coinRootId.color, 1.25))
            border.width: 5 * Theme.scaleW
            radius: width / 2
            Image {
                id: kingImgId
                source: "qrc:/Qml/Icon/crown.png"
                anchors.centerIn: parent
            }
        }
    }

    function updateState()
    {
        coinRootId.item = board.item(coinRootId.col, coinRootId.row)
        coinRootId.visible = (coinRootId.item.type !== Enums.TypeNone)
        coinRootId.dark = (coinRootId.item.player === Enums.PlayerB)
        kingImgId.visible = (coinRootId.item.type === Enums.King)
        draughtsPiece.color = (coinRootId.item.active) ? Theme.green : coinRootId.color
    }

    Connections {
        target: board
        function onDataChanged()
        {
            updateState()
        }
    }
}
