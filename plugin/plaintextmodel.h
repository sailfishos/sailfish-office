/*
 * Copyright (C) 2019 Jolla Ltd.
 * Contact: Andrew den Exter <andrew.den.exter@jolla.com>
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

#ifndef PLAINTEXTMODEL_H
#define PLAINTEXTMODEL_H

#include <QAbstractItemModel>
#include <QCache>
#include <QExplicitlySharedDataPointer>
#include <QFile>
#include <QTextStream>
#include <QUrl>

#include <vector>

class PlainTextModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int lineCount READ rowCount NOTIFY countChanged)
    Q_DISABLE_COPY(PlainTextModel)
public:
    static constexpr qint64 maximumFileSize = INT_MAX; // This is probably a little optimistic.
    static constexpr qint64 maximumSynchronousSize = 4000;
    static constexpr qint64 maximumCost = 16000;

    enum Roles {
        LineText
    };

    enum Status {
        Null,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status)

    explicit PlainTextModel(QObject *parent = nullptr);
    ~PlainTextModel();

    QUrl source() const;
    void setSource(const QUrl &source);

    Status status() const;

    Q_INVOKABLE QString textAt(int index) const;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;

signals:
    void sourceChanged();
    void statusChanged();
    void countChanged();

private:
    struct FileData;
    class Reader;
    class ReaderEvent;

    friend struct FileData;
    friend class Reader;
    friend class ReaderEvent;

    struct Line
    {
        Line() = default;
        Line(qint64 offset, qint64 length) : offset(offset), length(length) {}
        qint64 offset = 0;
        qint64 length = 0;
    };

    inline QString *lineAt(int index);
    inline static bool readLines(
            QTextStream *stream, std::vector<Line> *lines, std::vector<QString> *cache);

    std::vector<Line> m_lines;
    QCache<int, QString> m_textCache { maximumCost + (maximumCost / 20) };
    QExplicitlySharedDataPointer<FileData> m_fileData;
    QFile m_file;
    QTextStream m_textStream;
    QUrl m_source;
    qint64 m_cost =  0;
    Status m_status = Null;
};

#endif // DOCUMENTLISTMODEL_H
