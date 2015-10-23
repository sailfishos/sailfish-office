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

#ifndef DOCUMENTPROVIDERPLUGIN_H
#define DOCUMENTPROVIDERPLUGIN_H

#include <QObject>
#include <QUrl>
#include <QtGui/QImage>

class DocumentProviderPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QUrl icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QUrl thumbnail READ thumbnail  NOTIFY thumbnailChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)
    Q_PROPERTY(QUrl page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(QString setupPageUrl READ setupPageUrl NOTIFY titleChanged)
    Q_PROPERTY(bool needsSetup READ needsSetup NOTIFY needsSetupChanged)

public:
    DocumentProviderPlugin(QObject *parent = 0);
    virtual ~DocumentProviderPlugin();

    virtual int count() const = 0;
    virtual QUrl icon() const = 0;
    virtual QString title() const = 0;
    virtual QString description() const = 0;
    virtual QObject *model() const = 0;
    virtual QUrl thumbnail() const = 0;
    virtual bool isReady() const = 0;
    // Reimplement this and return the name of a QML file containing the account setup
    // for this plugin, if applicable. Otherwise a no-setup plugin will be assumed.
    virtual QString setupPageUrl() const;
    virtual bool needsSetup() const;

    QUrl page() const;
    void setPage(const QUrl &url);

    Q_INVOKABLE virtual void deleteFile(const QUrl &file);

signals:
    void countChanged();
    void iconChanged();
    void thumbnailChanged();
    void pageChanged();
    void titleChanged();
    void modelChanged();
    void readyChanged();
    void needsSetupChanged();

private:
    class Private;
    Private *d;
};
Q_DECLARE_INTERFACE(DocumentProviderPlugin, "DocumentProviderPluginInterface/1.0")

#endif // DOCUMENTPROVIDERPLUGIN_H
