import QtQuick 2.15
import QtQuick.Window 2.15

pragma Singleton

QtObject {
    id: myTheme

    property real scaleW
    property real scaleH
    property real scaleFont

    property int targetWidth: 1920
    property int targetHeight: 1200

    property color white: "#FFFFFF"
    property color black: "#000000"
    property color red: "#7B3A3A"
    property color green: "#4E7B3A"
    property color transparent: "transparent"
    property color boardBackground: "#5A3A1A"
    property color boardBackgroundLight: "#7B5B39"

    property color tileDark: "#774E2C"
    property color tileLight: "#D6B07C"
    property color coinLight: "#F5F5DC"
    property color coinDark: "#27221F" //"#1C1A15"

    property string fontFamily: "Roboto"

    Component.onCompleted: {
        myTheme.scaleH = (Screen.height/targetHeight);
        myTheme.scaleW = (Screen.width/targetWidth);
        myTheme.scaleFont = scaleH < scaleW ? scaleH : scaleW;
    }
}
