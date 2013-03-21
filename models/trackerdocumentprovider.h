/*
 *
 */

#ifndef TRACKERDOCUMENTPROVIDER_H
#define TRACKERDOCUMENTPROVIDER_H

#include "documentproviderplugin.h"

class TrackerDocumentProvider : public DocumentProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES(DocumentProviderPlugin)

public:
    TrackerDocumentProvider(QObject* parent = 0);
    ~TrackerDocumentProvider();

    virtual int count() const;
    virtual QUrl icon() const;
    virtual QString title() const;
    virtual QString description() const;
    virtual QObject *model() const;
    virtual QUrl thumbnail() const;
    virtual bool isReady() const;
private:
    class Private;
    Private* d;
};

#endif // TRACKERDOCUMENTPROVIDER_H
