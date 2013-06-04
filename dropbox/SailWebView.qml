import QtQuick 1.1
import QtWebKit 1.0
import Sailfish.Silica 1.0

Flickable {
    id: flickable
    contentWidth: Math.max(parent.width,webView.width)
    contentHeight: Math.max(parent.height,webView.height)
    anchors.fill: parent
    property alias url: webView.url
    TextField { id: vkb; visible: false; focus: false; }
    WebView {
        id: webView
        focus: true
        preferredWidth: flickable.width
        preferredHeight: flickable.height
        visible: !tryingText.visible;
        onLoadFinished: {
            if(url.toString().length === 85) {
                vkb.forceActiveFocus();
                vkb.visible = true;
                vkb.closeSoftwareInputPanel();
                authTry.start();
            }
            tryingText.visible = false;
        }
    }

    Timer {
        id: authTry;
        running: false;
        repeat: false;
        // 3 seconds being the psychological human "moment"
        interval: 3000;
        onTriggered: controllerMIT.oauth_v1_access_token();
    }

    Label {
        id: tryingText
        anchors.centerIn: parent;
        text: "Trying to load login page..."
        color: "Grey"
        z: 10
    }
}
