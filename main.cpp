 
#include <QApplication>
#include <QtDeclarative/QtDeclarative>
#include <QDeclarativeView>

#include <QtDBus/QDBusConnection>
#include <libjollasignonuiservice/signonuiservice.h>

#include "sailfishapplication/sailfishapplication.h"
#include "models/documentlistmodel.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    auto app = Sailfish::createApplication(argc, argv);
    auto view = Sailfish::createView("main.qml");

    // We want to have SignonUI in process, if user wants to create account from Documents
    SignonUiService *ssoui = new SignonUiService(0, true); // in process
    ssoui->setInProcessServiceName(QLatin1String("org.sailfish.documents"));
    ssoui->setInProcessObjectPath(QLatin1String("/SailfishDocumentsSignonUi"));

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    bool registeredService = sessionBus.registerService(QLatin1String("org.sailfish.gallery"));
    bool registeredObject = sessionBus.registerObject(QLatin1String("/SailfishDocumentsSignonUi"), ssoui,
            QDBusConnection::ExportAllContents);

    if (!registeredService || !registeredObject) {
        qWarning() << Q_FUNC_INFO << "CRITICAL: unable to register signon ui service:"
                   << QLatin1String("org.sailfish.documents") << "at object path:"
                   << QLatin1String("/SailfishDocumentsSignonUi");
    }

    view->rootContext()->setContextProperty("jolla_signon_ui_service", ssoui);

    Sailfish::showView(view);

    int retn = app->exec();

    if (registeredService)
        sessionBus.unregisterService(QLatin1String("org.sailfish.documents"));
    if (registeredObject)
        sessionBus.unregisterObject(QLatin1String("/SailfishDocumentsSignonUi"));
    delete ssoui;
    return retn;
}
