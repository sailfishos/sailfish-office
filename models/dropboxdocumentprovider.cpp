/*
 *
 */

#include "dropboxdocumentprovider.h"
#include "documentlistmodel.h"

#include "config.h"

class DropboxDocumentProvider::Private
{
public:
    Private()
        : model(new DocumentListModel)
        , needAuthenticate(true)
        , fileListModel(0)
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

        model->setObjectName("DropboxDocumentList");
    }
    ~Private() { model->deleteLater(); }
    DocumentListModel* model;
    QHash<QString, DocumentType> docTypes;
    bool needAuthenticate;
    QAbstractListModel* fileListModel;
};

DropboxDocumentProvider::DropboxDocumentProvider(QObject* parent)
    : DocumentProviderPlugin(parent)
    , d(new Private)
{
    setPage(QUrl::fromLocalFile(QString(DROPBOX_QML_PLUGIN_DIR).append("Sailfish/Office/Dropbox/FileNavigator.qml")));
}

DropboxDocumentProvider::~DropboxDocumentProvider()
{
    delete d;
}

bool DropboxDocumentProvider::isReady() const
{
    return true;
}

QUrl DropboxDocumentProvider::thumbnail() const
{
    return QUrl::fromLocalFile( IMAGES_INSTALL_DIR + QLatin1String("graphic-plugin-network.png") );
}

QObject* DropboxDocumentProvider::model() const
{
    return d->model;
}

QString DropboxDocumentProvider::description() const
{
    //: Description for dropbox files plugin
    //% "Files located on a dropbox"
    return qtTrId("sailfish-office-la-dropbox_description");
}

QString DropboxDocumentProvider::title() const
{
    //: Title for dropbox files plugin
    //% "Dropbox"
    return qtTrId("sailfish-office-he-dropbox_title");
}

QUrl DropboxDocumentProvider::icon() const
{
    return QUrl::fromLocalFile( IMAGES_INSTALL_DIR + QLatin1String("graphic-plugin-network.png") );
}

int DropboxDocumentProvider::count() const
{
    if(d->needAuthenticate)
        return -1;
    return -2;//d->fileListModel->rowCount(QModelIndex());
}

QString DropboxDocumentProvider::setupPageUrl() const
{
    return QString(DROPBOX_QML_PLUGIN_DIR).append("Sailfish/Office/Dropbox/SetupPage.qml");
}

void DropboxDocumentProvider::classBegin()
{

}

void DropboxDocumentProvider::componentComplete()
{

}

QObject* DropboxDocumentProvider::fileListModel() const
{
    return d->fileListModel;
}

void DropboxDocumentProvider::setFileListModel(QObject* fileListModel)
{
    if(qobject_cast<QAbstractListModel*>(fileListModel))
    {
        d->fileListModel = qobject_cast<QAbstractListModel*>(fileListModel);
        emit fileListModelChanged();
    }
}

bool DropboxDocumentProvider::needAuthenticate() const
{
    return d->needAuthenticate;
}

void DropboxDocumentProvider::setNeedAuthenticate(bool newValue)
{
    d->needAuthenticate = newValue;
    emit needAuthenticateChanged();
    emit countChanged();
}
