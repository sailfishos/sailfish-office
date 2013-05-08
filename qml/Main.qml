import QtQuick 1.1
import Sailfish.Silica 1.0
import org.calligra.CalligraComponents 0.1

ApplicationWindow
{
    id: window;
    initialPage: startPage;
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    //TODO: Convert all component usage to Qt.resolvedUrl once most development is done.
    Component { id: startPage; StartPage { } }

    //Preload Calligra plugins so we do not need to do that
    //when opening a document so opening becomes more responsive.
    Component.onCompleted: Calligra.loadPlugins();
}
