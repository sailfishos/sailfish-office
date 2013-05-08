/*
 *
 */

#ifndef SAILFISHOFFICEPLUGIN_H
#define SAILFISHOFFICEPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class SailfishOfficePlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT

public:
    explicit SailfishOfficePlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
    virtual void initializeEngine(QDeclarativeEngine* engine, const char* uri);
};

#endif // SAILFISHOFFICEPLUGIN_H
