/*
 *
 */

#ifndef TRACKERDOCUMENTPROVIDER_H
#define TRACKERDOCUMENTPROVIDER_H

#include "documentproviderplugin.h"
#include <QtCore/QFileInfo>
#include <QtCore/QRunnable>
#include <QtCore/QHash>
#include <QtQml/QQmlParserStatus>

class DocumentListModel;
class TrackerDocumentProvider : public DocumentProviderPlugin, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(DocumentProviderPlugin QQmlParserStatus)

public:
    enum DocumentType {
        TextDocumentType,
        SpreadsheetType,
        PresentationType
    };
    TrackerDocumentProvider(QObject* parent = 0);
    ~TrackerDocumentProvider();

    virtual int count() const;
    virtual QUrl icon() const;
    virtual QString title() const;
    virtual QString description() const;
    virtual QObject *model() const;
    virtual QUrl thumbnail() const;
    virtual bool isReady() const;

    virtual void classBegin();
    virtual void componentComplete();

public Q_SLOTS:
    void startSearch();
    void stopSearch();

private Q_SLOTS:
    void searchFinished();
    void trackerGraphChanged(const QString& className, const QVariantList&, const QVariantList& );

private:
    class Private;
    Private* d;
};

#endif // TRACKERDOCUMENTPROVIDER_H
