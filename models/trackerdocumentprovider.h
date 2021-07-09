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

#ifndef TRACKERDOCUMENTPROVIDER_H
#define TRACKERDOCUMENTPROVIDER_H

#include "documentprovider.h"

#include <QtQml/QQmlParserStatus>

class DocumentListModel;
class TrackerDocumentProvider : public DocumentProvider, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(DocumentProvider QQmlParserStatus)

public:
    enum DocumentType {
        TextDocumentType,
        SpreadsheetType,
        PresentationType
    };
    TrackerDocumentProvider(QObject *parent = 0);
    ~TrackerDocumentProvider();

    virtual int count() const;
    virtual QObject *model() const;
    virtual bool isReady() const;
    virtual bool error() const;

    virtual void classBegin();
    virtual void componentComplete();

    virtual void deleteFile(const QUrl &file) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void startSearch();
    void stopSearch();

private Q_SLOTS:
    void searchFinished();
    void trackerGraphChanged(const QString &graphName);

private:
    class Private;
    Private *d;
};

#endif // TRACKERDOCUMENTPROVIDER_H
