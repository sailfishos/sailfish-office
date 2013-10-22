/*
 *
 */

#include "dropboxplugin.h"

#include "controller.h"

#include <QQmlContext>
#include <QtGui/QGuiApplication>

#include "config.h"

DropboxPlugin::DropboxPlugin(QObject* parent)
    : QQmlExtensionPlugin(parent)
{
}

void DropboxPlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.Dropbox" ) );
}

void DropboxPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.Dropbox" ) );

    engine->addImportPath(DROPBOX_QML_PLUGIN_DIR);

    Controller* controller = new Controller(qApp);

    QQmlContext *context = engine->rootContext();
    context->setContextProperty("controllerMIT", controller);
    context->setContextProperty("Options", &controller->m_options);
    context->setContextProperty("folderListModel", controller->folder_model);
    context->setContextProperty("filesTransferModel", controller->filestransfer_model);

    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
