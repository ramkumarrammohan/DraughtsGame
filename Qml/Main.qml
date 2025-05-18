import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import com.checkerboard.Theme 1.0

import "Elements"

ApplicationWindow {
    visible: true
    width: Screen.height * 0.65
    height: Screen.height * 0.80
    title: qsTr("Draughts Game")

    color: Theme.boardBackground

    Column {
        anchors.fill: parent
        spacing: 10 * Theme.scaleH
        anchors.margins: 10 * Theme.scaleW

        // header item | warning content space
        HeaderItem {
            width: parent.width
            height: 50 * Theme.scaleH
        }

        // main board component
        MainBoard {
            width: parent.width
            height: width
        }

        // player highlighter here
        PlayerHighlighter {
            width: parent.width
            height: 50 * Theme.scaleH
        }

        // score card here
        ScoreCardItem {
            width: parent.width
            height: 50 * Theme.scaleH
        }
    }
}
