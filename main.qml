import QtQuick 1.1
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    id: window;
    initialPage: startPage;
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    Component { id: startPage; StartPage { } }
}
