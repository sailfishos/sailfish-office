 
#include <QApplication>
#include <QtDeclarative/QtDeclarative>
#include <QQuickView>
#include <QQmlError>

#include "sailfishapplication.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    // First of all, run kbuildsycoca to make sure the data is correct when we get to it.
    QProcess::startDetached(QLatin1String("/usr/bin/kbuildsycoca4"));

    auto app = Sailfish::createApplication(argc, argv);
    // Note, these must be said now, otherwise some plugins using QSettings
    // will get terribly confused when they fail to load properly.
    app->setOrganizationName("Sailfish");
    app->setApplicationName("Sailfish Office");

    auto view = Sailfish::createView("Main.qml");

    //% "Documents"
    Q_UNUSED(QT_TRID_NOOP("office-ap-name"))

    int retn = 1;
    if( !view->errors().count() > 0 )
    {
        Sailfish::showView(view);
        retn = app->exec();
    }

    return retn;
}
