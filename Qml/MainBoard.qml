import QtQuick 2.15

import com.checkerboard.Theme 1.0
import com.checkerboard.Enums 1.0

import "Elements"

Rectangle {
    id: rootBoardId
    color: Theme.boardBackground
    radius: 10 * Theme.scaleW

    Column {
        spacing: 0
        anchors.fill: parent

        Repeater {
            model: board !== null ? board.columnCount() : 0

            Row {
                spacing: 0
                property int yPos: index

                Repeater {
                    id: repeatr
                    model: board !== null ? board.rowCount() : 0

                    Rectangle {
                        width: rootBoardId.width / 10
                        height: rootBoardId.height / 10
                        property int xCord: index
                        property int yCord: yPos
                        color: (xCord + yCord) % 2 === 0 ? Theme.tileLight : Theme.tileDark;

                        Coin {
                            id: coinId
                            width: height
                            height: parent.height * 0.8
                            anchors.centerIn: parent
                            row: parent.xCord
                            col: parent.yCord
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if(board.isMoving)
                                {
                                    board.destination = Qt.point(yCord, xCord)
                                    board.move();
                                }
                                else if(board.activePlayer === coinId.item.player)
                                {
                                    board.setActiveItem(yCord, xCord)
                                    board.isMoving = true
                                    board.source = Qt.point(yCord, xCord)
                                }
                            }
                        }
                    }
                }
            } //Row
        }
    } //Column

    CPopup {
        id: gameOverItemId
        anchors.centerIn: parent
        visible: false
        onOkClicked: {
            board.reset()
            gameOverItemId.close()
        }
    }

    Connections {
        target: board
        function onGameDraw()
        {
            gameOverItemId.message = "Draw match\nPlay again..."
            gameOverItemId.open()
        }
        function onGameOver(player)
        {
            var pname = (player === Enums.PlayerA) ? "Player A" : "Player B"
            gameOverItemId.message = qsTr("!! Congratulations !!\n%1 won the match.").arg(pname)
            gameOverItemId.open()
        }
    }
}
