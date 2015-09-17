/*
 * Copyright (C) 2015 Damien Caliste
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

#ifndef TAGSTHREAD_H
#define TAGSTHREAD_H

#include <QtCore/QObject>

class TagsThreadJob : public QObject
{
    Q_OBJECT
public:
    TagsThreadJob( QString &path ): path(path) {};
    ~TagsThreadJob();

    void run();

    QList<QString> tags;
    QString path;
};

class TagsThreadPrivate;
class TagsThread : public QObject
{
    Q_OBJECT
public:
    TagsThread( QObject* parent = 0 );
    ~TagsThread();

    void queueJob( TagsThreadJob *job );
    void cancelJob( TagsThreadJob *job );

Q_SIGNALS:
    void jobFinished( TagsThreadJob *job );

private:
    friend class TagsThreadPrivate;
    TagsThreadPrivate * const priv;
};

#endif // TAGSTHREAD_H
