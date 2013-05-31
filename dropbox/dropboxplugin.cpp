/*
 *
 */

#include "dropboxplugin.h"

#include "controller.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtGui/QApplication>

#include "config.h"

DropboxPlugin::DropboxPlugin(QObject* parent)
    : QDeclarativeExtensionPlugin(parent)
{
}

void DropboxPlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.Dropbox" ) );
}

void DropboxPlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office.Dropbox" ) );

    engine->addImportPath(CALLIGRA_QML_PLUGIN_DIR);

    Controller* controller = new Controller(qApp);

    QDeclarativeContext *context = engine->rootContext();
    context->setContextProperty("controllerMIT", controller);
    context->setContextProperty("Options", &controller->m_options);
    context->setContextProperty("folderListModel", controller->folder_model);
    context->setContextProperty("filesTransferModel", controller->filestransfer_model);

    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
}

Q_EXPORT_PLUGIN2(dropboxplugin, DropboxPlugin)
