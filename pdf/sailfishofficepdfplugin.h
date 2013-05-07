/*
 *
 */

#ifndef SAILFISHOFFICEPDFPLUGIN_H
#define SAILFISHOFFICEPDFPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class SailfishOfficePDFPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT

public:
    explicit SailfishOfficePDFPlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
};

#endif // SAILFISHOFFICEPDFPLUGIN_H
