/*
 *
 */

#ifndef SAILFISHOFFICEPDFPLUGIN_H
#define SAILFISHOFFICEPDFPLUGIN_H

#include <QQmlExtensionPlugin>

class SailfishOfficePDFPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    explicit SailfishOfficePDFPlugin(QObject* parent = 0);

    virtual void registerTypes(const char* uri);
};

#endif // SAILFISHOFFICEPDFPLUGIN_H
