#include "trackerdocumentprovider.h"
#include "documentlistmodel.h"
#include <QDir>
#include <QtCore/qthreadpool.h>

#include <qglobal.h>

#include <QSparqlConnection>
#include <QSparqlResult>
#include <QSparqlError>

#include "config.h"


const QString SearchThread::textDocumentType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PaginatedTextDocument");
const QString SearchThread::presentationType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#PresentationDocument");
const QString SearchThread::spreadsheetType = QString("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#SpreadsheetDocument");

SearchThread::SearchThread(DocumentListModel* model, const QHash<QString, TrackerDocumentProvider::DocumentType> &docTypes, QObject *parent) 
    : QObject(parent), m_model(model), m_abort(false), m_docTypes(docTypes)
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    // Get documents from the device's tracker instance
    QSparqlConnection connection("QTRACKER");
    QSparqlQuery query(
        "PREFIX nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> "
        "PREFIX nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
        "PREFIX nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
        "SELECT ?name ?path ?size ?lastAccessed ?mimeType " // ?lastModified ?type ?uuid "
        "WHERE { "
        "?u nfo:fileName ?name . "
        "?u nie:url ?path . "
        "?u nfo:fileSize ?size . "
        "?u nfo:fileLastAccessed ?lastAccessed . "
        "?u nie:mimeType ?mimeType . "
//        "?u nfo:fileLastModified ?lastModified . "
//        "?u rdf:type ?type . "
//        "?u nie:isStoredAs ?uuid . "
        "{ ?u a nfo:PaginatedTextDocument } UNION { ?u a nfo:Presentation } UNION { ?u a nfo:Spreadsheet }"
    " }");
    QSparqlResult* result = connection.exec(query);
    result->waitForFinished();
    if(!result->hasError())
    {
        while (result->next() && !m_abort) {
            //qDebug() << result->binding(0).value().toString() << result->binding(4).value().toString();
            
            //CMDocumentListModel::DocumentInfo info;
            m_model->addItem(
                result->binding(0).value().toString(),
                result->binding(1).value().toString(),
                result->binding(1).value().toString().split('.').last(),
                result->binding(2).value().toInt(),
                result->binding(3).value().toDateTime(),
                result->binding(4).value().toString()
            );
//             info.fileName = ;
//             info.filePath = ;
//             info.docType = m_docTypes.value(info.filePath.split('.').last());
//             info.fileSize = result->binding(2).value().toString();
//             info.authorName = "-";
//             info.accessedTime = ;
//             info.modifiedTime = result->binding(4).value().toDateTime();
//             info.uuid = result->binding(6).value().toString();
            /*QString type = result->binding(5).value().toString();
        qDebug() << type;
        //.split(',').last();
        if(type == textDocumentType) {
                info.docType = CMDocumentListModel::TextDocumentType;
            } else if(type == presentationType) {
                info.docType = CMDocumentListModel::PresentationType;
            } else if(type == spreadsheetType) {
                info.docType = CMDocumentListModel::SpreadsheetType;
            } else {
                info.docType = CMDocumentListModel::UnknownType;
            }

        qDebug() << result->binding(6).value();
        qDebug() << result->binding(7).value();
        qDebug() << result->binding(8).value();*/

            //emit documentFound(info);
        }
        emit finished();
        return;
    }
    else
        qDebug() << "Error while querying Tracker:" << result->lastError().message();
}

class TrackerDocumentProvider::Private {
public:
    Private()
        : model(new DocumentListModel)
        , searchThread(0)
    {
        docTypes["odt"] = TextDocumentType;
        docTypes["doc"] = TextDocumentType;
        docTypes["docx"] = TextDocumentType;
        docTypes["odp"] = PresentationType;
        docTypes["ppt"] = PresentationType;
        docTypes["pptx"] = PresentationType;
        docTypes["ods"] = SpreadsheetType;
        docTypes["xls"] = SpreadsheetType;
        docTypes["xlsx"] = SpreadsheetType;

        model->setObjectName("TrackerDocumentList");
//         QDir directory;
//         directory.setPath( "/home/nemo/Documents" );
//         QFileInfoList entries = directory.entryInfoList( QStringList() << "*.odt" << "*.ods" << "*.odp", QDir::Files, QDir::Name );
// 
//         foreach(const QFileInfo& info, entries) {
//             model->addItem(info.fileName(), info.filePath(), info.fileName().split(".").last(), info.size(), info.lastRead());
//         }
    }
    ~Private() { model->deleteLater(); }
    DocumentListModel* model;
    QHash<QString, DocumentType> docTypes;
    SearchThread* searchThread;
};

TrackerDocumentProvider::TrackerDocumentProvider(QObject* parent)
    : DocumentProviderPlugin(parent)
    , d(new Private)
{
}

TrackerDocumentProvider::~TrackerDocumentProvider()
{
    stopSearch();
    delete d;
}

void TrackerDocumentProvider::classBegin()
{}

void TrackerDocumentProvider::componentComplete()
{
    startSearch();
}

void TrackerDocumentProvider::startSearch()
{
    if (d->searchThread) {
        qDebug() << "Already searching or finished search";
        return;
    }
    d->searchThread = new SearchThread(d->model, d->docTypes);
    //connect(m_searchThread, SIGNAL(documentFound(CMDocumentListModel::DocumentInfo)), this, SLOT(addDocument(CMDocumentListModel::DocumentInfo)));
    connect(d->searchThread, SIGNAL(finished()), this, SLOT(searchFinished()));
    d->searchThread->setAutoDelete(false);
    QThreadPool::globalInstance()->start(d->searchThread);
}

void TrackerDocumentProvider::stopSearch()
{
    if (d->searchThread)
        d->searchThread->abort();
}

void TrackerDocumentProvider::searchFinished()
{
    Q_ASSERT(d->searchThread);
    d->searchThread->deleteLater();
    d->searchThread = 0;
    emit countChanged();
}

int TrackerDocumentProvider::count() const
{
    // TODO lolnope
    return d->model->rowCount(QModelIndex());
}

QString TrackerDocumentProvider::description() const
{
    //: Description for local device files plugin
    //% "Files found on this device."
    return qtTrId("sailfish-office-la-localfiles_description");
}

QUrl TrackerDocumentProvider::icon() const
{
    return QUrl::fromLocalFile( IMAGES_INSTALL_DIR + QLatin1String("graphic-plugin-local.png") );
}

bool TrackerDocumentProvider::isReady() const
{
    return true;
}

QObject* TrackerDocumentProvider::model() const
{
    return d->model;
}

QUrl TrackerDocumentProvider::thumbnail() const
{
    return QUrl::fromLocalFile( IMAGES_INSTALL_DIR + QLatin1String("graphic-plugin-local.png") );
}

QString TrackerDocumentProvider::title() const
{
    //: Title for local device files plugin
    //% "This Device"
    return qtTrId("sailfish-office-he-localfiles_title");
}
