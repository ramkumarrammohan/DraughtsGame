import QtQuick 2.15

import com.checkerboard.Theme 1.0

Rectangle {
    width: parent.width
    height: 50 * Theme.scaleH
    radius: 10 * Theme.scaleW
    color: Theme.boardBackgroundLight

    Row {
        anchors.fill: parent
        // anchors.margins: 2 * Theme.scaleW

        CTextBold {
            text: board ? board.playerAScore : 0
            color: Theme.white
            font.pixelSize: 22 * Theme.scaleFont
            width: (parent.width - moveItemId.width) / 2
            height: parent.height
        }

        Rectangle {
            id: moveItemId
            width: 100 * Theme.scaleW
            height: parent.height
            color: "#e0d4b3"

            CTextBold {
                anchors.centerIn: parent
                text: qsTr("Moves\n%1").arg(board ? parseInt(board.moveCount/2)
                                                  : 0)
                font.pixelSize: 16 * Theme.scaleFont
                color: Theme.boardBackground
            }
        }

        CTextBold {
            text: board ? board.playerBScore : 0
            color: Theme.white
            font.pixelSize: 22 * Theme.scaleFont
            width: (parent.width - moveItemId.width) / 2
            height: parent.height
        }
    }
}
