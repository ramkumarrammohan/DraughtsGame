import QtQuick 2.15
import QtQuick.Controls 2.15

import com.checkerboard.Theme 1.0

Popup {
    id: popupId
    width: 300 * Theme.scaleW
    height: 150 * Theme.scaleH
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose

    signal okClicked
    property string message: ""

    background: Rectangle {
        anchors.fill: parent
        color: Theme.boardBackgroundLight
        radius: 10 * Theme.scaleW
    }

    Column {
        id: contentColumnId
        width: parent.width - (10 * Theme.scaleW)
        spacing: 20 * Theme.scaleH

        CTextBold {
            text: qsTr("%1").arg(message)
            width: parent.width
            height: contentHeight
            color: Theme.white
            wrapMode: Text.WordWrap
            font.pixelSize: 22 * Theme.scaleFont
        }

        CButton {
            id: buttonId
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("OK")
            fontColor: Theme.white
            backgroundColor: Theme.boardBackground
            onClicked: {
                okClicked()
            }
        }
    }
}
