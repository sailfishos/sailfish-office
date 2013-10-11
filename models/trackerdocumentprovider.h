/*
 *
 */

#ifndef TRACKERDOCUMENTPROVIDER_H
#define TRACKERDOCUMENTPROVIDER_H

#include "documentproviderplugin.h"
#include <QtCore/QFileInfo>
#include <QtCore/QRunnable>
#include <QtCore/QHash>
#include </usr/include/qt5/QtQml/QQmlParserStatus>

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
    void searchFinished();
private:
    class Private;
    Private* d;
};

class SearchThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SearchThread(DocumentListModel* model, const QHash< QString, TrackerDocumentProvider::DocumentType >& docTypes, QObject* parent = 0);
    ~SearchThread();

    void run();
    void abort() { m_abort = true; }

signals:
    void documentFound(const QFileInfo& fileInfo);
    void finished();

private:
    DocumentListModel* m_model;
    bool m_abort;
    QHash<QString, TrackerDocumentProvider::DocumentType> m_docTypes;
    static const QString textDocumentType;
    static const QString presentationType;
    static const QString spreadsheetType;
};
#endif // TRACKERDOCUMENTPROVIDER_H
