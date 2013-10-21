import QtQuick 2.0
import Sailfish.Silica 1.0
import org.kde.calligra 1.0 as Calligra

ApplicationWindow
{
    id: window;
    initialPage: startPage;
    cover: "";

    //TODO: Convert all component usage to Qt.resolvedUrl once most development is done.
    Component { id: startPage; StartPage { } }

    //Preload Calligra plugins so we do not need to do that
    //when opening a document so opening becomes more responsive.
    Component.onCompleted: Calligra.loadPlugins();
}
