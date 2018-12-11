import QtQuick 2.0
import QtQuick.Window 2.2
import QtWebEngine 1.0

Window {
    visible: true
    visibility: Window.Maximized
    WebEngineView {
        anchors.fill: parent
        url: "qrc:/echoclient.html"
    }
}
