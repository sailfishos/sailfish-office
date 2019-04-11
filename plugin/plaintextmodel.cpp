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

#include "plaintextmodel.h"

#include <QCoreApplication>
#include <QEvent>
#include <QQmlInfo>
#include <QThread>
#include <silicatheme.h>

class PlainTextModel::FileData : public QSharedData
{
public:
    FileData(PlainTextModel *model)
        : fileName(model->m_file.fileName())
        , model(model)
    {
    }

    QString fileName;
    PlainTextModel *model = nullptr;
};

class PlainTextModel::Reader : public QThread
{
public:
    inline Reader(const QExplicitlySharedDataPointer<PlainTextModel::FileData> &fileData);

    inline void run() override;

    QExplicitlySharedDataPointer<PlainTextModel::FileData> m_fileData;
};

class PlainTextModel::ReaderEvent : public QEvent
{
public:
    inline ReaderEvent(const QExplicitlySharedDataPointer<PlainTextModel::FileData> &fileData);
    inline ~ReaderEvent();

    const QExplicitlySharedDataPointer<PlainTextModel::FileData> fileData;
    std::vector<PlainTextModel::Line> lines;
    std::vector<QString> text;
    bool atEnd = false;
};

PlainTextModel::PlainTextModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

PlainTextModel::~PlainTextModel()
{
    if (m_fileData) {
        m_fileData->model = nullptr;
    }
}

QUrl PlainTextModel::source() const
{
    return m_source;
}

void PlainTextModel::setSource(const QUrl &source)
{
    if (m_source != source) {
        const Status previousStatus = m_status;
        const size_t previousCount = m_lines.size();

        m_source = source;

        if (!m_lines.empty()) {
            beginRemoveRows(QModelIndex(), 0, m_lines.size() - 1);
            m_cost = 0;
            m_lines.clear();
            endRemoveRows();
        }

        m_textStream.setDevice(nullptr);
        m_file.close();

        if (m_fileData) {
            m_fileData->model = nullptr;
            m_fileData.reset();
        }

        if (m_source.isEmpty()) {
            m_status = Null;
        } else if (!m_source.isLocalFile()) {
            qmlInfo(this) << m_source << " is not a local file";
            m_status = Error;
        } else {
            m_file.setFileName(m_source.toLocalFile());

            if (m_file.size() == 0) {
                m_status = Ready;
            } else if (m_file.size() < 0 || m_file.size() > maximumFileSize) {
                qmlInfo(this) << "File is too large " << m_source << ": " << m_file.errorString();
                m_status = Error;
            } else if (!m_file.open(QIODevice::ReadOnly)) {
                qmlInfo(this) << "Can't open " << m_source << ": " << m_file.errorString();
                m_status = Error;
            } else {
                m_textStream.setDevice(&m_file);

                if (m_file.size() > maximumSynchronousSize) {
                    m_fileData = new FileData(this);

                    Reader *const reader = new Reader(m_fileData);
                    reader->start();

                    m_status = Loading;
                } else {
                    std::vector<Line> lines;
                    std::vector<QString> text;

                    readLines(&m_textStream, &lines, &text);

                    if (!lines.empty()) {
                        beginInsertRows(QModelIndex(), 0, lines.size() - 1);

                        m_lines = std::move(lines);

                        int index = 0;
                        for (const QString &lineText : text) {
                            m_textCache.insert(index++, new QString(std::move(lineText)), lineText.length());
                        }

                        endInsertRows();
                    }

                    m_status = Ready;
                }
            }
        }

        if (previousStatus != m_status) {
            emit statusChanged();
        }
        if (previousCount != m_lines.size()) {
            emit countChanged();
        }
        emit sourceChanged();
    }
}

PlainTextModel::Status PlainTextModel::status() const
{
    return m_status;
}

QString PlainTextModel::textAt(int index) const
{
    QString * const line = const_cast<PlainTextModel *>(this)->lineAt(index);
    return line ? *line : QString();
}

QHash<int, QByteArray> PlainTextModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames = {
        { LineText, "lineText" }
    };
    return roleNames;
}

QVariant PlainTextModel::data(const QModelIndex &index, int role) const
{
    if (QString * const line = role == LineText
            ? static_cast<QString *>(index.internalPointer())
            : nullptr) {
        return *line;
    } else if (index.isValid() && role == LineText) {
        return QString();
    } else {
        return QVariant();
    }
}

QModelIndex PlainTextModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && row >=0 && row < int(m_lines.size()) && column == 0
            ? createIndex(row, column, const_cast<PlainTextModel *>(this)->lineAt(row))
            : QModelIndex();
}

QModelIndex PlainTextModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int PlainTextModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_lines.size() : 0;
}

int PlainTextModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? 1 : 0;
}

QString *PlainTextModel::lineAt(int index)
{
    if (index < 0 || index >= int(m_lines.size())) {
        return nullptr;
    } else if (QString * const text = m_textCache.object(index)) {
        return text;
    } else {
        const Line &line = m_lines.at(index);

        if (line.length == 0) {
            return nullptr;
        } else {
            m_textStream.seek(line.offset);

            QString * const text = new QString(Silica::Theme::_encodeTextLinks(m_textStream.readLine()));
            m_textCache.insert(index, text, text->length());

            return text;
        }
    }
}

bool PlainTextModel::readLines(
        QTextStream *stream, std::vector<Line> *lines, std::vector<QString> *cache)
{
    const qint64 reserveSize = std::min(maximumCost, stream->device()->size()) / 80;

    lines->reserve(reserveSize);
    if (cache) {
        cache->reserve(reserveSize);
    }

    for (qint64 cost = 0; cost < maximumCost;) {
        if (stream->atEnd()) {
            return true;
        }

        const qint64 offset = stream->pos();
        stream->seek(offset);   // This forces a flush or something to that effect. Without it each read-line gets progressively slower.
        QString text = stream->readLine();

        lines->push_back({ offset, text.length() });
        if (cache) {
            cache->push_back(Silica::Theme::_encodeTextLinks(text));
        }

        cost += text.length();
    }
    return false;
}

PlainTextModel::Reader::Reader(const QExplicitlySharedDataPointer<PlainTextModel::FileData> &fileData)
    : m_fileData(fileData)
{
    connect(this, &QThread::finished, this, &QObject::deleteLater);
}

void PlainTextModel::Reader::run()
{
    QFile file(m_fileData->fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        ReaderEvent * const event = new ReaderEvent(m_fileData);
        event->atEnd = true;
        QCoreApplication::postEvent(this, event);
    } else {
        QTextStream stream(&file);

        bool atEnd = false;
        for (bool cache = true; m_fileData->model && !atEnd; cache = false) {
            ReaderEvent * const event = new ReaderEvent(m_fileData);

            atEnd = PlainTextModel::readLines(&stream, &event->lines, cache ? &event->text : nullptr);
            event->atEnd = atEnd;

            QCoreApplication::postEvent(this, event);
        }
    }
}

PlainTextModel::ReaderEvent::ReaderEvent(
        const QExplicitlySharedDataPointer<PlainTextModel::FileData> &fileData)
    : QEvent(QEvent::None)
    , fileData(fileData)
{
}

PlainTextModel::ReaderEvent::~ReaderEvent()
{
    if (fileData->model) {
        if (!lines.empty()) {
            size_t first = fileData->model->m_lines.size();
            size_t last = first + lines.size() - 1;

            fileData->model->beginInsertRows(QModelIndex(), first, last);

            fileData->model->m_lines.reserve(last);
            fileData->model->m_lines.insert(fileData->model->m_lines.end(), lines.begin(), lines.end());

            for (const QString &lineText : text) {
                fileData->model->m_textCache.insert(first++, new QString(std::move(lineText)), lineText.length());
            }

            fileData->model->endInsertRows();
            emit fileData->model->countChanged();
        }

        if (atEnd) {
            fileData->model->m_status = Ready;
            emit fileData->model->statusChanged();
        }
    }
}
