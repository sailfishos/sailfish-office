import QtQuick 2.0
import QtWebKit.experimental 1.0
import Sailfish.Silica 1.0

WebView {
    id: webView
    anchors.fill: parent

    experimental.userAgent: "Mozilla/5.0 (Linux; U; like Android 4.0.3; ko-kr; Jolla Build/Alpha) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
    experimental.onLoadVisuallyCommitted: {
        tryingText.visible = false;
        if(url.toString().length === 41) controllerMIT.oauth_v1_access_token()
        if(url.toString().length === 140) experimental.test.touchTap(webView, 100, 150)
    }

    Label {
        id: tryingText
        anchors.centerIn: parent;
        text: "Trying to load login page..."
        color: "Grey"
        z: 10
    }
}


