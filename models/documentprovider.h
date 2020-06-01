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

#ifndef DOCUMENTPROVIDER_H
#define DOCUMENTPROVIDER_H

#include <QObject>
#include <QUrl>

class DocumentProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(bool error READ error NOTIFY errorChanged)

public:
    DocumentProvider(QObject *parent = 0);
    virtual ~DocumentProvider();

    virtual int count() const = 0;
    virtual QObject *model() const = 0;
    virtual bool isReady() const = 0;
    virtual bool error() const = 0;

    Q_INVOKABLE virtual void deleteFile(const QUrl &file);

signals:
    void countChanged();
    void modelChanged();
    void readyChanged();
    void errorChanged();

private:
    class Private;
    Private *d;
};

Q_DECLARE_INTERFACE(DocumentProvider, "DocumentProviderInterface/1.0")

#endif // DOCUMENTPROVIDER_H
