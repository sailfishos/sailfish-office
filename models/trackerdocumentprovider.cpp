#include "trackerdocumentprovider.h"
#include "documentlistmodel.h"

class TrackerDocumentProvider::Private {
public:
    Private()
        : model(new DocumentListModel)
    {
        model->setObjectName("TrackerDocumentList");
        model->setPath("/home/nemo/Documents");
    }
    ~Private() { model->deleteLater(); }
    DocumentListModel* model;
};

TrackerDocumentProvider::TrackerDocumentProvider(QObject* parent)
    : DocumentProviderPlugin(parent)
    , d(new Private)
{
}

TrackerDocumentProvider::~TrackerDocumentProvider()
{
    delete d;
}

int TrackerDocumentProvider::count() const
{
    // TODO lolnope
    return 15;
}

QString TrackerDocumentProvider::description() const
{
    return QString("Docs stored in tracker or somesuch description");
}

QUrl TrackerDocumentProvider::icon() const
{
    return QUrl::fromLocalFile("/some/icon.png");
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
    // TODO a cached icon of some of the contents, created by the model...
    return QUrl::fromLocalFile("/some/icon.png");
}

QString TrackerDocumentProvider::title() const
{
    return QString("Tracker docs");
}

#include "trackerdocumentprovider.moc"
