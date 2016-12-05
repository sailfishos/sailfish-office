/*
 * Copyright (C) 2016 Damien Caliste
 * Contact: Damien Caliste <dcaliste@free.fr>
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

#include "trackertagprovider.h"

#include <QSparqlConnection>
#include <QSparqlResult>
#include <QSparqlError>

//The Tracker driver to use.
static const QString trackerDriver{"QTRACKER"};

class TrackerTagProvider::Private {
public:
    Private()
        : connection(new QSparqlConnection(trackerDriver))
    {
    }

    ~Private() {
        delete connection;
    }

    QSparqlConnection *connection;
};

TrackerTagProvider::TrackerTagProvider(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

TrackerTagProvider::~TrackerTagProvider()
{
    delete d;
}

void TrackerTagProvider::loadTags(const QString &path)
{
    QSparqlQuery q(QString("SELECT ?label WHERE {"
                           " ?f nie:isStoredAs ?p ; nao:hasTag ?tag ."
                           " ?p nie:url '%1' ."
                           " ?tag a nao:Tag ; nao:prefLabel ?label ."
                           "} ORDER BY ASC(?label)").arg(QString(path).replace('\'', "\\\'")));
    QSparqlResult* result = d->connection->exec(q);
    result->setProperty("path", QVariant(path));
    connect(result, &QSparqlResult::finished, this, &TrackerTagProvider::loadTagFinished);
}

void TrackerTagProvider::addTag(const QString &path, const QString &tag)
{
    // First, check if tag exists.
    QSparqlQuery q(QString("SELECT ?tag WHERE {"
                           " ?tag a nao:Tag ; nao:prefLabel '%1' ."
                           "} ORDER BY ASC(?label)").arg(QString(tag).replace('\'', "\\\'")));
    QSparqlResult* result = d->connection->exec(q);
    result->setProperty("path", QVariant(path));
    result->setProperty("tag", QVariant(tag));
    connect(result, &QSparqlResult::finished, this, &TrackerTagProvider::existTagFinished);
}

void TrackerTagProvider::addExistingTag(const QString &path, const QString &tag)
{
    QSparqlQuery q(QString("INSERT {"
                           " ?f nao:hasTag ?tag"
                           "} WHERE {"
                           " ?f nie:isStoredAs ?p ."
                           " ?p nie:url '%1' ."
                           " ?tag nao:prefLabel '%2'"
                           "}").arg(QString(path).replace('\'', "\\\'")).arg(QString(tag).replace('\'', "\\\'")), QSparqlQuery::InsertStatement);
    QSparqlResult* result = d->connection->exec(q);
    connect(result, &QSparqlResult::finished, this, &TrackerTagProvider::addTagFinished);
}

void TrackerTagProvider::addNewTag(const QString &path, const QString &tag)
{
    QSparqlQuery q(QString("INSERT {"
                           " _:tag a nao:Tag ; nao:prefLabel '%2' ."
                           " ?f nao:hasTag _:tag"
                           "} WHERE {"
                           " ?f nie:isStoredAs ?p ."
                           " ?p nie:url '%1' ."
                           "}").arg(QString(path).replace('\'', "\\\'")).arg(QString(tag).replace('\'', "\\\'")), QSparqlQuery::InsertStatement);
    QSparqlResult* result = d->connection->exec(q);
    connect(result, &QSparqlResult::finished, this, &TrackerTagProvider::addTagFinished);
}

void TrackerTagProvider::removeTag(const QString &path, const QString &tag)
{
    QSparqlQuery q(QString("DELETE {"
                           " ?f nao:hasTag ?tag"
                           "} WHERE {"
                           " ?f nie:isStoredAs ?p ."
                           " ?p nie:url '%1' ."
                           " ?tag nao:prefLabel '%2' ."
                           "}").arg(QString(path).replace('\'', "\\\'")).arg(QString(tag).replace('\'', "\\\'")), QSparqlQuery::DeleteStatement);
    QSparqlResult* result = d->connection->exec(q);
    connect(result, &QSparqlResult::finished, this, &TrackerTagProvider::removeTagFinished);
}

void TrackerTagProvider::loadTagFinished()
{
    QSparqlResult *r = qobject_cast<QSparqlResult*>(sender());
    QList<QString> tags;

    if (!r->hasError()) {
        while (r->next()) {
            tags.append(r->binding(0).value().toString());
        }

        emit tagLoaded(r->property("path").toString(), tags);
    }

    r->deleteLater();
}

void TrackerTagProvider::existTagFinished()
{
    QSparqlResult *r = qobject_cast<QSparqlResult*>(sender());

    if (!r->hasError()) {
        if (r->next()) {
            addExistingTag(r->property("path").toString(), r->property("tag").toString());
        } else {
            addNewTag(r->property("path").toString(), r->property("tag").toString());
        }
    }

    r->deleteLater();
}

void TrackerTagProvider::addTagFinished()
{
    QSparqlResult *r = qobject_cast<QSparqlResult*>(sender());

    r->deleteLater();
}

void TrackerTagProvider::removeTagFinished()
{
    QSparqlResult *r = qobject_cast<QSparqlResult*>(sender());

    r->deleteLater();
}
