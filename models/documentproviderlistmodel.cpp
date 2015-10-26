/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jolla.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "documentproviderlistmodel.h"
#include "documentproviderplugin.h"
#include "documentlistmodel.h"

#include <QQmlComponent>
#include <QDebug>

class DocumentProviderListModel::Private
{
public:
    Private(DocumentProviderListModel *qq)
        : q(qq)
        , completed(false)
        , albumDelegate(0)
    {
        roles[Title] = "title";
        roles[Description] = "description";
        roles[Icon] = "icon";
        roles[Count] = "count";
        roles[Thumbnail] = "thumbnail";
        roles[Page] = "page";
        roles[SetupPageURL] = "setupPageUrl";
        roles[Ready] = "ready";
        roles[ProviderModel] = "providerModel";
        roles[NeedsSetup] = "needsSetup";
    }

    DocumentProviderListModel *q;
    QHash<int, QByteArray> roles;
    bool completed;
    QList<DocumentProviderPlugin*> providers;
    QQmlComponent* albumDelegate;

    static void source_append(QQmlListProperty<DocumentProviderPlugin> *property, DocumentProviderPlugin *source)
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
        QObject::connect(source, SIGNAL(needsSetupChanged()), q, SLOT(sourceInfoChanged()));

        if (source->isReady())
            q->updateActiveSources();
    }

    static int source_count(QQmlListProperty<DocumentProviderPlugin> *property)
    {
        Private *d = static_cast<Private *>(property->data);
        return d->providers.count();
    }

    static DocumentProviderPlugin *source_at(QQmlListProperty<DocumentProviderPlugin> *property, int index)
    {
        Private *d = static_cast<Private *>(property->data);
        return d->providers.at(index);
    }

    static void source_clear(QQmlListProperty<DocumentProviderPlugin> *property)
    {
        Private *d = static_cast<Private *>(property->data);
        d->providers.clear();
    }
};

DocumentProviderListModel::DocumentProviderListModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
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
    if (d->completed)
        return;
    d->completed = true;

    // find all potential providers
    // create instance and store in d->providers
}

QQmlListProperty< DocumentProviderPlugin > DocumentProviderListModel::sources()
{
    return QQmlListProperty<DocumentProviderPlugin>(
                this,
                d,
                DocumentProviderListModel::Private::source_append,
                DocumentProviderListModel::Private::source_count,
                DocumentProviderListModel::Private::source_at,
                DocumentProviderListModel::Private::source_clear);
}

void DocumentProviderListModel::updateActiveSources()
{
}

void DocumentProviderListModel::sourceInfoChanged()
{
    DocumentProviderPlugin* source = qobject_cast< DocumentProviderPlugin* >(sender());
    int index = d->providers.indexOf(source);
    if (index > -1) {
        QModelIndex changedIndex = createIndex(index, 0);
        dataChanged(changedIndex, changedIndex);
    }
}

QQmlComponent* DocumentProviderListModel::albumDelegate() const
{
    return d->albumDelegate;
}

void DocumentProviderListModel::setAlbumDelegate(QQmlComponent *albumDelegate)
{
    if (d->albumDelegate != albumDelegate) {
        d->albumDelegate = albumDelegate;
        emit albumDelegateChanged();
    }
}

QModelIndex DocumentProviderListModel::index(int row, int column, const QModelIndex &parent) const
{
    return QAbstractListModel::index(row, column, parent);
}

QVariant DocumentProviderListModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid()) {
        if (index.row() > -1 && index.row() < d->providers.count()) {
            const DocumentProviderPlugin* provider = d->providers.at(index.row());
            switch(role) {
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
            case SetupPageURL:
                result.setValue<QUrl>(QUrl::fromLocalFile(provider->setupPageUrl()));
                break;
            case Ready:
                result.setValue<bool>(provider->isReady());
                break;
            case ProviderModel:
                result.setValue<QObject*>(provider->model());
                //qDebug() << qobject_cast<DocumentListModel*>(provider->model())->rowCount(QModelIndex());
                break;
            case NeedsSetup:
                result.setValue<bool>(provider->needsSetup());
                break;
            default:
                result.setValue<QString>(QLatin1String("Unknown role in DocumentProviderListModel::data"));
                break;
            }
        }
    }
    return result;
}

int DocumentProviderListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return d->providers.count();
}

QHash<int, QByteArray> DocumentProviderListModel::roleNames() const
{
    return d->roles;
}
