import QtQuick 2.15

import com.checkerboard.Theme 1.0
import com.checkerboard.Enums 1.0

Rectangle {
    width: parent.width
    height: 50 * Theme.scaleH
    color: Theme.boardBackgroundLight
    radius: 10 * Theme.scaleW

    Row {
        anchors.fill: parent
        anchors.margins: 2 * Theme.scaleW

        Rectangle {
            width: parent.width / 2
            height: parent.height
            radius: 10 * Theme.scaleW
            color: (board ? (board.activePlayer === Enums.PlayerA ? Theme.boardBackground
                                                         : Theme.transparent)
                         : Theme.transparent)
            Row {
                anchors.centerIn: parent
                spacing: 10 * Theme.scaleW
                Rectangle {
                    width: height
                    height: parent.height * 0.7
                    radius: height / 2
                    color: Theme.white
                    anchors.verticalCenter: parent.verticalCenter
                }
                CTextBold {
                    text: qsTr("Player A")
                    color: Theme.white
                    font.pixelSize: 20 * Theme.scaleFont
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Rectangle {
            width: parent.width / 2
            height: parent.height
            radius: 10 * Theme.scaleW
            color: (board ? (board.activePlayer === Enums.PlayerB ? Theme.boardBackground
                                                                  : Theme.transparent)
                          : Theme.transparent)
            Row {
                anchors.centerIn: parent
                spacing: 10 * Theme.scaleW
                CTextBold {
                    text: qsTr("Player B")
                    color: Theme.white
                    font.pixelSize: 20 * Theme.scaleFont
                    anchors.verticalCenter: parent.verticalCenter
                }
                Rectangle {
                    width: height
                    height: parent.height * 0.7
                    radius: height / 2
                    color: Theme.black
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
