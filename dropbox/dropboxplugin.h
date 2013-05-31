/*
 *
 */

#ifndef DROPBOXPLUGIN_H
#define DROPBOXPLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

class DropboxPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT

public:
    explicit DropboxPlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QDeclarativeEngine* engine, const char* uri);
};

#endif // DROPBOXPLUGIN_H
