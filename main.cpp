 
#include <QApplication>
#include <QtDeclarative/QtDeclarative>
#include <QDeclarativeView>

#include "sailfishapplication.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    // First of all, run kbuildsycoca to make sure the data is correct when we get to it.
    QProcess::startDetached(QLatin1String("/usr/bin/kbuildsycoca4"));

    auto app = Sailfish::createApplication(argc, argv);
    auto view = Sailfish::createView("Main.qml");

    int retn = 1;
    if( !view->errors().count() > 0 )
    {
        Sailfish::showView(view);
        retn = app->exec();
    }

    return retn;
}
