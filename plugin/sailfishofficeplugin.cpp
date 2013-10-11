/*
 *
 */

#include "sailfishofficeplugin.h"

#include <QtDeclarative/QtDeclarative>

#include <QTranslator>
#include <QApplication>

#include "config.h"

class Translator : public QTranslator
{
public:
    Translator(QObject *parent)
        : QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    ~Translator()
    {
        qApp->removeTranslator(this);
    }
};

SailfishOfficePlugin::SailfishOfficePlugin( QObject* parent )
    : QQmlExtensionPlugin( parent )
{

}

void SailfishOfficePlugin::registerTypes(const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office" ) );
}

void SailfishOfficePlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_ASSERT( uri == QLatin1String( "Sailfish.Office" ) );

    engine->addImportPath(CALLIGRA_QML_PLUGIN_DIR);

    Translator *engineeringEnglish = new Translator(engine);
    engineeringEnglish->load("sailfish-office_eng_en", TRANSLATION_INSTALL_DIR);

    Translator *translator = new Translator(engine);
    translator->load(QLocale(), "sailfish-office", "-", TRANSLATION_INSTALL_DIR);

    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

