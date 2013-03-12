import QtQuick 1.1
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    initialPage: fileListPage;
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    
    Component { id: fileListPage; FileListPage { } }
}
