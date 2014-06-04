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
#include <QDir>
#include <QGraphicsObject>
#include <QDebug>

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QDBusConnection>
#include <QTranslator>
#include <QLocale>

#include <MDeclarativeCache>

#include <signonuiservice.h>

#include "sailfishapplication.h"
#include "config.h"
#include "models/documentlistmodel.h"
#include "models/trackerdocumentprovider.h"
#include "models/documentproviderplugin.h"
#include "models/documentproviderlistmodel.h"
#include "dbusadaptor.h"
#include <QtDBus/QDBusConnection>

QSharedPointer<QApplication> Sailfish::createApplication(int &argc, char **argv)
{
    auto app = QSharedPointer<QApplication>{new QApplication{argc, argv}};
    //FIXME: We should be able to use a pure QGuiApplication but currently too much of
    //Calligra depends on QApplication.
    //QSharedPointer<QGuiApplication>(MDeclarativeCache::qApplication(argc, argv));

    QTranslator* engineeringEnglish = new QTranslator( app.data() );
    if( !engineeringEnglish->load("sailfish-office_eng_en", TRANSLATION_INSTALL_DIR) )
        qWarning( "Could not load engineering english translation file!");
    QCoreApplication::installTranslator( engineeringEnglish );

    QTranslator* translator = new QTranslator( app.data() );
    if( !translator->load( QLocale::system(), "sailfish-office", "-", TRANSLATION_INSTALL_DIR) )
        qWarning( ("Could not load translations for " + QLocale::system().name()).toLatin1() );
    QCoreApplication::installTranslator( translator );

    return app;
}

QSharedPointer<QQuickView> Sailfish::createView(const QString &file)
{
    qmlRegisterType< DocumentListModel >( "Sailfish.Office.Files", 1, 0, "DocumentListModel" );
    qmlRegisterType< DocumentProviderListModel >( "Sailfish.Office.Files", 1, 0, "DocumentProviderListModel" );
    qmlRegisterType< TrackerDocumentProvider >( "Sailfish.Office.Files", 1, 0, "TrackerDocumentProvider" );
    qmlRegisterInterface< DocumentProviderPlugin >( "DocumentProviderPlugin" );

    QSharedPointer<QQuickView> view(MDeclarativeCache::qQuickView());
    view->engine()->addImportPath(CALLIGRA_QML_PLUGIN_DIR);
    view->setSource(QUrl::fromLocalFile(QML_INSTALL_DIR + file));

    new DBusAdaptor{view.data()};

    if(!QDBusConnection::sessionBus().registerObject("/org/sailfish/office/ui", view.data()))
        qWarning() << "Could not register /org/sailfish/office/ui D-Bus object.";

    if(!QDBusConnection::sessionBus().registerService("org.sailfish.office"))
        qWarning() << "Could not register org.sailfish.office D-Bus service.";

    return view;
}

void Sailfish::showView(const QSharedPointer<QQuickView> &view) 
{
    view->showFullScreen();
}


