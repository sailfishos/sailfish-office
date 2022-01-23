/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#include "djvutocmodel.h"

//#include <poppler-qt5.h>
#include "djvu.h"
#include <QDebug>
#include <QThread>
#include <QtXml/QDomDocument>

struct DJVUTocEntry
{
    DJVUTocEntry()
        : level(0)
        , pageNumber(0)
    {}
    QString title;
    int level;
    int pageNumber;
};

class TocThread: public QThread
{
    Q_OBJECT
public:
    TocThread(KDjVu *document, QObject *parent = 0)
        : QThread(parent), m_document(document)
    {
    }
    ~TocThread()
    {
        wait();
        qDeleteAll(entries);
    }

    QList<DJVUTocEntry*> entries;

    void run()
    {
        const QDomDocument *toc = m_document->documentBookmarks(); //toc();
        addSynopsisChildren(toc, 0);
        delete toc;
        emit tocAvailable();
    }

    void addSynopsisChildren(const QDomNode *parent, int level)
    {
        if (!parent || parent->isNull())
            return;

        // keep track of the current listViewItem
        QDomNode n = parent->firstChild();
        while (!n.isNull()) {
            DJVUTocEntry *tocEntry = new DJVUTocEntry;
            tocEntry->level = level;
            // convert the node to an element (sure it is)
            QDomElement e = n.toElement();
            tocEntry->title = e.tagName();
/*
            // Apparently we can have external links in the ToC.
            // Not doing this for now, but leave it in here as a note to self
            // if (!e.attribute("ExternalFileName").isNull()) item.setAttribute("ExternalFileName", e.attribute("ExternalFileName"));
            if (!e.attribute("DestinationName").isNull()) {
                Poppler::LinkDestination *dest = m_document->linkDestination(e.attribute("DestinationName"));
                if (dest) {
                    tocEntry->pageNumber = dest->pageNumber();
                    delete dest;
                }
                //item.setAttribute("ViewportName", e.attribute("DestinationName"));
            }
            if (!e.attribute("Destination").isNull()) {
                //fillViewportFromLinkDestination( vp, Poppler::LinkDestination(e.attribute("Destination")) );
                //item.setAttribute( "Viewport", vp.toString() );
                Poppler::LinkDestination dest(e.attribute("Destination"));
                tocEntry->pageNumber = dest.pageNumber();
            }
            // if (!e.attribute("Open").isNull()) item.setAttribute("Open", e.attribute("Open"));
            // if (!e.attribute("DestinationURI").isNull()) item.setAttribute("URL", e.attribute("DestinationURI"));
*/
            // Add the entry to the list of ToC entries
            entries.append(tocEntry);
            // descend recursively and advance to the next node
            ++level;
            if (e.hasChildNodes())
                addSynopsisChildren(&n, level);
            --level;
            n = n.nextSibling();
        }
    }

signals:
    void tocAvailable();

private:
    KDjVu *m_document;
};

class DJVUTocModel::Private
{
public:
     Private(KDjVu *doc)
        : document(doc)
        , tocReady(false)
        , tocThread(nullptr)
    {}
    ~Private() { delete tocThread; }

    KDjVu *document;
    bool tocReady;
    TocThread *tocThread;
};

DJVUTocModel::DJVUTocModel(KDjVu *document, QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(document))
{
}

DJVUTocModel::~DJVUTocModel()
{
    delete d;
}

QHash<int, QByteArray> DJVUTocModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Title] = "title";
    names[Level] = "level";
    names[PageNumber] = "pageNumber";
    return names;
}

QVariant DJVUTocModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid() && d->tocReady) {
        int row = index.row();
        if (row > -1 && row < d->tocThread->entries.count()) {
            const DJVUTocEntry *entry = d->tocThread->entries.at(row);
            switch(role)
            {
            case Title:
                result.setValue<QString>(entry->title);
                break;
            case Level:
                result.setValue<qint32>(entry->level);
                break;
            case PageNumber:
                result.setValue<qint32>(entry->pageNumber);
                break;
            default:
                result.setValue<QString>(QString("Unknown role: %1").arg(role));
                break;
            }
        }
    }
    return result;
}

int DJVUTocModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !d->tocReady)
        return 0;
    return d->tocThread->entries.count();
}

int DJVUTocModel::count() const
{
    return (d->tocReady) ? d->tocThread->entries.count() : 0;
}

bool DJVUTocModel::ready() const
{
    return d->tocReady;
}

void DJVUTocModel::requestToc()
{
    if (d->tocThread)
        return;

    d->tocThread = new TocThread(d->document);
    d->tocThread->start();
    connect(d->tocThread, &TocThread::tocAvailable,
            this, &DJVUTocModel::onTocAvailable);
}

void DJVUTocModel::onTocAvailable()
{
    d->tocReady = true;
    emit readyChanged();

    beginInsertRows(QModelIndex(), 0, d->tocThread->entries.count() - 1);
    endInsertRows();
    emit countChanged();
}

#include "djvutocmodel.moc"
