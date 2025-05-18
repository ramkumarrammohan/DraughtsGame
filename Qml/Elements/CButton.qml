import QtQuick 2.15
import QtQuick.Controls 2.15

import com.checkerboard.Theme 1.0

Button {
    id: buttonId
    width: contentItemId.width
    height: contentItemId.height

    property color fontColor: Theme.white
    property color backgroundColor: Theme.white

    background:  Rectangle {
        id: backgroundId
        anchors.fill: parent
        radius: 10 * Theme.scaleW
        color: backgroundColor
    }
    contentItem: Item{}
    CTextBold {
        id: contentItemId
        text: qsTr(buttonId.text)
        width: contentWidth + (30 * Theme.scaleW)
        height: contentHeight + (20 * Theme.scaleH)
        font.pixelSize: 16 * Theme.scaleFont
        color: fontColor
    }
    onPressed: backgroundId.color = Qt.darker(backgroundColor, 1.2)
    onReleased: backgroundId.color = backgroundColor
}
