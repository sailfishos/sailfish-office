 
#include <QApplication>
#include <QGuiApplication>
#include <QProcess>
#include <QQuickView>
#include <QQmlError>
#include <QQmlContext>
#include "sailfishapplication.h"

class CoverWindowFetcher : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QQuickWindow *coverWindow() {
        foreach (QWindow *w, qGuiApp->allWindows()) {
            if (QQuickWindow *qw = qobject_cast<QQuickWindow *>(w)) {
                if (w->inherits("DeclarativeCoverWindow"))
                    return qw;
            }
        }
        return 0;
    }
};

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    // First of all, run kbuildsycoca to make sure the data is correct when we get to it.
    QProcess::startDetached(QLatin1String("/usr/bin/kbuildsycoca4"));

    // TODO: start using Silica booster
    QQuickWindow::setDefaultAlphaBuffer(true);

    auto app = Sailfish::createApplication(argc, argv);
    // Note, these must be said now, otherwise some plugins using QSettings
    // will get terribly confused when they fail to load properly.
    app->setOrganizationName("Sailfish");
    app->setApplicationName("Sailfish Office");

    auto view = Sailfish::createView("Main.qml");

    QQmlContext *context = view->rootContext();
    context->setContextProperty(QStringLiteral("coverWindowAccessor"), new CoverWindowFetcher);
    context->setContextProperty(QStringLiteral("applicationWindow"), view.data());

    //% "Documents"
    Q_UNUSED(QT_TRID_NOOP("sailfish-office-ap-name"))

    int retn = 1;
    if( !view->errors().count() > 0 )
    {
        Sailfish::showView(view);
        retn = app->exec();
    }

    return retn;
}

#include "main.moc"
