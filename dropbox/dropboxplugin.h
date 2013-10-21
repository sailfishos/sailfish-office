/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

class DropboxPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Sailfish.Office.DropboxPlugin")
public:
    explicit DropboxPlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QQmlEngine* engine, const char* uri);
};

#endif // DROPBOXPLUGIN_H
