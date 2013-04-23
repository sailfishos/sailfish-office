#include "documentproviderlistmodel.h"
#include "documentproviderplugin.h"
#include "documentlistmodel.h"

#include <QDeclarativeComponent>
#include <QDebug>

class DocumentProviderListModel::Private
{
public:
    Private(DocumentProviderListModel* qq)
        : q(qq)
        , completed(false)
        , albumDelegate(0)
    {}
    DocumentProviderListModel* q;
    bool completed;
    QList<DocumentProviderPlugin*> providers;
    QDeclarativeComponent* albumDelegate;

    static void source_append(QDeclarativeListProperty<DocumentProviderPlugin> *property, DocumentProviderPlugin *source)
    {
        Private *d = static_cast<Private *>(property->data);
        DocumentProviderListModel *q = static_cast<DocumentProviderListModel *>(property->object);
        d->providers.append(source);
        QObject::connect(source, SIGNAL(countChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(iconChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(thumbnailChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(pageChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(titleChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(modelChanged()), q, SLOT(sourceInfoChanged()));
        QObject::connect(source, SIGNAL(readyChanged()), q, SLOT(sourceInfoChanged()));

        if (source->isReady())
            q->updateActiveSources();
    }

    static int source_count(QDeclarativeListProperty<DocumentProviderPlugin> *property)
    {
        Private *d = static_cast<Private *>(property->data);
        return d->providers.count();
    }

    static DocumentProviderPlugin *source_at(QDeclarativeListProperty<DocumentProviderPlugin> *property, int index)
    {
        Private *d = static_cast<Private *>(property->data);
        return d->providers.at(index);
    }
};

DocumentProviderListModel::DocumentProviderListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    QHash<int, QByteArray> roles;
    roles[Title] = "title";
    roles[Description] = "description";
    roles[Icon] = "icon";
    roles[Count] = "count";
    roles[Thumbnail] = "thumbnail";
    roles[Page] = "page";
    roles[Ready] = "ready";
    roles[ProviderModel] = "providerModel";
    setRoleNames(roles);
}

DocumentProviderListModel::~DocumentProviderListModel()
{
    delete d;
}

void DocumentProviderListModel::classBegin()
{
}

void DocumentProviderListModel::componentComplete()
{
    if(d->completed)
        return;
    d->completed = true;

    // find all potential providers
    // create instance and store in d->providers
}

QDeclarativeListProperty< DocumentProviderPlugin > DocumentProviderListModel::sources()
{
    return QDeclarativeListProperty<DocumentProviderPlugin>(
                this,
                d,
                DocumentProviderListModel::Private::source_append,
                DocumentProviderListModel::Private::source_count,
                DocumentProviderListModel::Private::source_at);
}

void DocumentProviderListModel::updateActiveSources()
{

}

void DocumentProviderListModel::sourceInfoChanged()
{
    DocumentProviderPlugin* source = qobject_cast< DocumentProviderPlugin* >(sender());
    int index = d->providers.indexOf(source);
    if(index > -1) {
        QModelIndex changedIndex = createIndex(index, 0);
        dataChanged(changedIndex, changedIndex);
    }
}

QDeclarativeComponent* DocumentProviderListModel::albumDelegate() const
{
    return d->albumDelegate;
}

void DocumentProviderListModel::setAlbumDelegate(QDeclarativeComponent* albumDelegate)
{
    if (d->albumDelegate != albumDelegate) {
        d->albumDelegate = albumDelegate;
        emit albumDelegateChanged();
    }
}

QModelIndex DocumentProviderListModel::index(int row, int column, const QModelIndex& parent) const
{
    return QAbstractListModel::index(row, column, parent);
}

QVariant DocumentProviderListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid())
    {
        if(index.row() > -1 && index.row() < d->providers.count())
        {
            const DocumentProviderPlugin* provider = d->providers.at(index.row());
            switch(role)
            {
                case Title:
                    result.setValue<QString>(provider->title());
                    break;
                case Description:
                    result.setValue<QString>(provider->description());
                    break;
                case Icon:
                    result.setValue<QUrl>(provider->icon());
                    break;
                case Count:
                    result.setValue<int>(provider->count());
                    break;
                case Thumbnail:
                    result.setValue<QUrl>(provider->thumbnail());
                    break;
                case Page:
                    result.setValue<QUrl>(provider->page());
                    break;
                case Ready:
                    result.setValue<bool>(provider->isReady());
                    break;
                case ProviderModel:
                    result.setValue<QObject*>(provider->model());
                    qDebug() << qobject_cast<DocumentListModel*>(provider->model())->rowCount(QModelIndex());
                    break;
                default:
                    result.setValue<QString>(QLatin1String("Unknown role in DocumentProviderListModel::data"));
                    break;
            }
        }
    }
    return result;
}

int DocumentProviderListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->providers.count();
}
