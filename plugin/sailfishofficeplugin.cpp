/*
 *
 */

#include "sailfishofficeplugin.h"
#include "linkarea.h"

#include <QtDeclarative/QtDeclarative>

#include "config.h"

SailfishOfficePlugin::SailfishOfficePlugin( QObject* parent )
    : QDeclarativeExtensionPlugin( parent )
{

}

void SailfishOfficePlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office" ) );
    qmlRegisterType<LinkArea>( uri, 1, 0, "LinkArea" );
}

void SailfishOfficePlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office" ) );

    engine->addImportPath(CALLIGRA_QML_PLUGIN_DIR);

    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
}

Q_EXPORT_PLUGIN2(sailfishofficeplugin, SailfishOfficePlugin)
