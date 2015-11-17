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

#ifndef DOCUMENTPROVIDERLISTMODEL_H
#define DOCUMENTPROVIDERLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlListProperty>
#include </usr/include/qt5/QtQml/QQmlParserStatus>

QT_BEGIN_NAMESPACE
class QQmlComponent;
QT_END_NAMESPACE

class DocumentProvider;
class DocumentProviderListModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<DocumentProvider> sources READ sources)
    Q_PROPERTY(int count READ rowCount NOTIFY sourcesChanged)
    Q_PROPERTY(QQmlComponent *albumDelegate READ albumDelegate WRITE setAlbumDelegate NOTIFY albumDelegateChanged)
    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "sources")
public:
    enum ProviderRoles {
        Title = Qt::UserRole + 1,
        Description,
        Icon,
        Count,
        Thumbnail,
        Page,
        SetupPageURL,
        Ready,
        ProviderModel,
        NeedsSetup
    };

    explicit DocumentProviderListModel(QObject *parent = 0);
    virtual ~DocumentProviderListModel();

    virtual void classBegin();
    virtual void componentComplete();

    QQmlListProperty<DocumentProvider> sources();

    QQmlComponent *albumDelegate() const;
    void setAlbumDelegate(QQmlComponent *albumDelegate);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash<int, QByteArray> roleNames() const;

Q_SIGNALS:
    void sourcesChanged();
    void albumDelegateChanged();

private slots:
    void updateActiveSources();
    void sourceInfoChanged();

private:
    class Private;
    Private *d;
};

#endif // DOCUMENTPROVIDERLISTMODEL_H
