/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QApplication>
#include <QGuiApplication>
#include <QtQml>
#include <QQuickView>
#include <QQmlError>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDBusConnection>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

#include <MDeclarativeCache>

#include "config.h"
#include "models/filtermodel.h"
#include "models/documentlistmodel.h"
#include "models/trackerdocumentprovider.h"
#include "models/documentprovider.h"
#include "models/documentproviderlistmodel.h"
#include "dbusadaptor.h"

namespace {
QSharedPointer<QApplication> createApplication(int &argc, char **argv)
{
    auto app = QSharedPointer<QApplication>(new QApplication(argc, argv));
    //FIXME: We should be able to use a pure QGuiApplication but currently too much of
    //Calligra depends on QApplication.
    //QSharedPointer<QGuiApplication>(MDeclarativeCache::qApplication(argc, argv));

    QTranslator *engineeringEnglish = new QTranslator(app.data());
    if (!engineeringEnglish->load("sailfish-office_eng_en", TRANSLATION_INSTALL_DIR))
        qWarning("Could not load engineering english translation file!");
    QCoreApplication::installTranslator(engineeringEnglish);

    QTranslator *translator = new QTranslator(app.data());
    if (!translator->load(QLocale::system(), "sailfish-office", "-", TRANSLATION_INSTALL_DIR))
        qWarning() << "Could not load translations for" << QLocale::system().name().toLatin1();
    QCoreApplication::installTranslator(translator);

    return app;
}

QSharedPointer<QQuickView> createView(const QString &file)
{
    qmlRegisterType<DocumentListModel>("Sailfish.Office.Files", 1, 0, "DocumentListModel");
    qmlRegisterType<DocumentProviderListModel>("Sailfish.Office.Files", 1, 0, "DocumentProviderListModel");
    qmlRegisterType<TrackerDocumentProvider>("Sailfish.Office.Files", 1, 0, "TrackerDocumentProvider");
    qmlRegisterType<FilterModel>("Sailfish.Office.Files", 1, 0, "FilterModel");
    qmlRegisterInterface<DocumentProvider>("DocumentProvider");

    QSharedPointer<QQuickView> view(MDeclarativeCache::qQuickView());
    view->engine()->addImportPath(CALLIGRA_QML_PLUGIN_DIR);
    view->setSource(QUrl::fromLocalFile(QML_INSTALL_DIR + file));

    new DBusAdaptor(view.data());

    if (!QDBusConnection::sessionBus().registerObject("/org/sailfish/office/ui", view.data()))
        qWarning() << "Could not register /org/sailfish/office/ui D-Bus object.";

    if (!QDBusConnection::sessionBus().registerService("org.sailfish.office"))
        qWarning() << "Could not register org.sailfish.office D-Bus service.";

    return view;
}
}

class CoverWindowFetcher : public QObject
{
    Q_OBJECT
public:
    CoverWindowFetcher(QObject *parent)
        : QObject(parent) {
    }

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
    // TODO: start using Silica booster
    QQuickWindow::setDefaultAlphaBuffer(true);

    auto app = createApplication(argc, argv);
    // Note, these must be said now, otherwise some plugins using QSettings
    // will get terribly confused when they fail to load properly.
    app->setOrganizationName("Sailfish");
    app->setApplicationName("Sailfish Office");

    auto view = createView("Main.qml");

    QQmlContext *context = view->rootContext();
    context->setContextProperty(QStringLiteral("coverWindowAccessor"), new CoverWindowFetcher(view->engine()));
    context->setContextProperty(QStringLiteral("applicationWindow"), view.data());

    //% "Documents"
    Q_UNUSED(QT_TRID_NOOP("sailfish-office-ap-name"))

    int retn = 1;
    if (!view->errors().count() > 0) {
        view->showFullScreen();
        retn = app->exec();
    }

    return retn;
}

#include "main.moc"
