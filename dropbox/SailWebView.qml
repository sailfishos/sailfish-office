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
            if(url.toString().match(/signup_tag/g)) {
                flickable.contentX=500
            } else {
                flickable.contentX=195
                flickable.contentY=50
                contentsScale=0.9
            }
            if(url.toString().length === 41) {
                vkb.forceActiveFocus();
                vkb.visible = true;
                vkb.closeSoftwareInputPanel()
                controllerMIT.oauth_v1_access_token()
            }
            tryingText.visible = false;
        }
    }

    Label {
        id: tryingText
        anchors.centerIn: parent;
        text: "Trying to load login page..."
        color: "Grey"
        z: 10
    }
}
