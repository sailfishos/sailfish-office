/*
 *
 */

#ifndef DROPBOXDOCUMENTPROVIDER_H
#define DROPBOXDOCUMENTPROVIDER_H

#include "documentproviderplugin.h"
#include <QDeclarativeParserStatus>

class DocumentListModel;
class DropboxDocumentProvider : public DocumentProviderPlugin, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(DocumentProviderPlugin QDeclarativeParserStatus)

public:
    enum DocumentType {
        TextDocumentType,
        SpreadsheetType,
        PresentationType
    };
    DropboxDocumentProvider(QObject* parent = 0);
    ~DropboxDocumentProvider();
    virtual bool isReady() const;
    virtual QUrl thumbnail() const;
    virtual QObject* model() const;
    virtual QString description() const;
    virtual QString title() const;
    virtual QUrl icon() const;
    virtual int count() const;
    virtual QString setupPageUrl() const;

    virtual void classBegin();
    virtual void componentComplete();

    void setFileListModel(QObject* fileListModel);

private:
    class Private;
    Private* d;
};

#endif // DROPBOXDOCUMENTPROVIDER_H
