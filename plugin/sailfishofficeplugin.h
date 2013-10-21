/*
 *
 */

#ifndef SAILFISHOFFICEPLUGIN_H
#define SAILFISHOFFICEPLUGIN_H

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

class SailfishOfficePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Sailfish.Office")
public:
    explicit SailfishOfficePlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QQmlEngine* engine, const char* uri);
};

#endif // SAILFISHOFFICEPLUGIN_H
