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

#include "trackerdocumentprovider.h"
#include "documentlistmodel.h"

#include <QDir>
#include <QtCore/qthreadpool.h>
#include <QtCore/QModelIndex>
#include <QtDBus/QDBusConnection>

#include <qglobal.h>

#include <QSparqlConnection>
#include <QSparqlResult>
#include <QSparqlError>

#include "config.h"

//The Tracker driver to use.
static const QString trackerDriver{"QTRACKER"};

//The query to run to get files out of Tracker.
static const QString documentQuery{
"PREFIX nfo: <http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#> "
"PREFIX nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
"PREFIX nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
"SELECT ?name ?path ?size ?lastAccessed ?mimeType WHERE { "
    "?u nfo:fileName ?name . "
    "?u nie:url ?path . "
    "?u nfo:fileSize ?size . "
    "?u nfo:fileLastAccessed ?lastAccessed . "
    "?u nie:mimeType ?mimeType . "
    "{ ?u a nfo:PaginatedTextDocument } UNION { ?u a nfo:Presentation } UNION { ?u a nfo:Spreadsheet } "
"}"
};

//Strings used for the DBus connection to listen to Tracker's GraphUpdated signal.
static const QString dbusService{"org.freedesktop.Tracker1"};
static const QString dbusPath{"/org/freedesktop/Tracker1/Resources"};
static const QString dbusInterface{"org.freedesktop.Tracker1.Resources"};
static const QString dbusSignal{"GraphUpdated"};

//The semantic class for all document types.
static const QString documentClassName("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#Document");

class TrackerDocumentProvider::Private {
public:
    Private()
        : model(new DocumentListModel)
        , connection{nullptr}
        , ready(false)
    {
        model->setObjectName("TrackerDocumentList");
    }

    ~Private() {
        model->deleteLater();
    }

    DocumentListModel *model;
    QSparqlConnection *connection;
    bool ready;
};

TrackerDocumentProvider::TrackerDocumentProvider(QObject *parent)
    : DocumentProvider(parent)
    , d(new Private)
{
}

TrackerDocumentProvider::~TrackerDocumentProvider()
{
    delete d->connection;
    delete d;
}

void TrackerDocumentProvider::classBegin()
{
}

void TrackerDocumentProvider::componentComplete()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    sessionBus.connect(dbusService, dbusPath, dbusInterface, dbusSignal, this, SLOT(trackerGraphChanged(QString,QVariantList,QVariantList)));

    d->connection = new QSparqlConnection(trackerDriver);
    startSearch();
}

void TrackerDocumentProvider::startSearch()
{
    QSparqlQuery q(documentQuery);
    QSparqlResult* result = d->connection->exec(q);
    connect(result, SIGNAL(finished()), this, SLOT(searchFinished()));

}

void TrackerDocumentProvider::stopSearch()
{
}

void TrackerDocumentProvider::searchFinished()
{
    QSparqlResult *r = qobject_cast<QSparqlResult*>(sender());
    if (!r->hasError()) {
        // d->model->clear();
        // Mark all current entries in the model dirty.
        d->model->setAllItemsDirty(true);
        while (r->next()) {
            // This will remove the dirty flag for already
            // existing entries.
            d->model->addItem(
                r->binding(0).value().toString(),
                r->binding(1).value().toString(),
                r->binding(1).value().toString().split('.').last(),
                r->binding(2).value().toInt(),
                r->binding(3).value().toDateTime(),
                r->binding(4).value().toString()
            );
        }
        // Remove all entries with the dirty mark.
        d->model->removeItemsDirty();
        if (!d->ready) {
            d->ready = true;
            emit readyChanged();
        }
    }

    emit countChanged();
}

int TrackerDocumentProvider::count() const
{
    // TODO lolnope
    return d->model->rowCount(QModelIndex());
}

QString TrackerDocumentProvider::description() const
{
    //: Description for local device files provider
    //% "Files found on this device."
    return qtTrId("sailfish-office-la-localfiles_description");
}

QUrl TrackerDocumentProvider::icon() const
{
    return QUrl();
}

bool TrackerDocumentProvider::isReady() const
{
    return d->ready;
}

QObject* TrackerDocumentProvider::model() const
{
    return d->model;
}

QUrl TrackerDocumentProvider::thumbnail() const
{
    return QUrl();
}

QString TrackerDocumentProvider::title() const
{
    //: Title for local device files provider
    //% "This Device"
    return qtTrId("sailfish-office-he-localfiles_title");
}

void TrackerDocumentProvider::deleteFile(const QUrl &file)
{
    if (QFile::exists(file.toLocalFile())) {
        QFile::remove(file.toLocalFile());

        const int count = d->model->rowCount(QModelIndex());
        for (int i = 0; i < count; ++i) {
            if (d->model->data(d->model->index(i, 0), DocumentListModel::FilePathRole).toUrl() == file) {
                d->model->removeAt(i);
                break;
            }
        }
    }
}

void TrackerDocumentProvider::trackerGraphChanged(const QString &className, const QVariantList&, const QVariantList&)
{
    if (className == documentClassName) {
        startSearch();
    }
}
