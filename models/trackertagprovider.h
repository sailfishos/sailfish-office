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

#ifndef TRACKERTAGPROVIDER_H
#define TRACKERTAGPROVIDER_H

#include <QtCore/QObject>

class TrackerTagProvider : public QObject
{
    Q_OBJECT

public:
    TrackerTagProvider(QObject *parent = 0);
    ~TrackerTagProvider();

    void loadTags(const QString &path);
    void addTag(const QString &path, const QString &tag);
    void removeTag(const QString &path, const QString &tag);

signals:
    void tagLoaded(const QString &path, const QList<QString> &tags);

private Q_SLOTS:
    void loadTagFinished();
    void existTagFinished();
    void addTagFinished();
    void removeTagFinished();

private:
    class Private;
    Private *d;

    void addNewTag(const QString &path, const QString &tag);
    void addExistingTag(const QString &path, const QString &tag);
};

#endif
