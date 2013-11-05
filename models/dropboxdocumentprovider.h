/*
 *
 */

#ifndef DROPBOXDOCUMENTPROVIDER_H
#define DROPBOXDOCUMENTPROVIDER_H

#include "documentproviderplugin.h"
#include </usr/include/qt5/QtQml/QQmlParserStatus>

class DocumentListModel;
class DropboxDocumentProvider : public DocumentProviderPlugin, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(DocumentProviderPlugin QQmlParserStatus)
    Q_PROPERTY(bool needAuthenticate READ needAuthenticate WRITE setNeedAuthenticate NOTIFY needAuthenticateChanged)
    Q_PROPERTY(QObject* fileListModel READ fileListModel WRITE setFileListModel NOTIFY fileListModelChanged)

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
    virtual bool needsSetup() const;

    virtual void classBegin();
    virtual void componentComplete();

    QObject* fileListModel() const;
    void setFileListModel(QObject* fileListModel);

    bool needAuthenticate() const;
    void setNeedAuthenticate(bool newValue);

Q_SIGNALS:
    void fileListModelChanged();
    void needAuthenticateChanged();

private:
    class Private;
    Private* d;
};

#endif // DROPBOXDOCUMENTPROVIDER_H
