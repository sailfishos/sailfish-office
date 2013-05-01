/*
 *
 */

#include "pdftocmodel.h"

#include <poppler-qt4.h>
#include <QDebug>

struct PDFTocEntry
{
    PDFTocEntry()
        : level(0)
        , pageNumber(0)
    {}
    QString title;
    int level;
    int pageNumber;
};

class PDFTocModel::Private
{
public:
    Private()
    {}

    QList<PDFTocEntry*> entries;
    Poppler::Document* document;

    void addSynopsisChildren( QDomNode * parent, int level )
    {
        // keep track of the current listViewItem
        QDomNode n = parent->firstChild();
        while( !n.isNull() )
        {
            PDFTocEntry* tocEntry = new PDFTocEntry();
            tocEntry->level = level;
            // convert the node to an element (sure it is)
            QDomElement e = n.toElement();
            tocEntry->title = e.tagName();

            // Apparently we can have external links in the ToC.
            // Not doing this for now, but leave it in here as a note to self
            // if (!e.attribute("ExternalFileName").isNull()) item.setAttribute("ExternalFileName", e.attribute("ExternalFileName"));
            if (!e.attribute("DestinationName").isNull()) {
                tocEntry->pageNumber = document->linkDestination(e.attribute("DestinationName"))->pageNumber();
                //item.setAttribute("ViewportName", e.attribute("DestinationName"));
            }
            if (!e.attribute("Destination").isNull())
            {
                //fillViewportFromLinkDestination( vp, Poppler::LinkDestination(e.attribute("Destination")) );
                //item.setAttribute( "Viewport", vp.toString() );
                Poppler::LinkDestination dest(e.attribute("Destination"));
                tocEntry->pageNumber = dest.pageNumber();
            }
            // if (!e.attribute("Open").isNull()) item.setAttribute("Open", e.attribute("Open"));
            // if (!e.attribute("DestinationURI").isNull()) item.setAttribute("URL", e.attribute("DestinationURI"));

            // Add the entry to the list of ToC entries
            entries.append(tocEntry);
            // descend recursively and advance to the next node
            ++level;
            if ( e.hasChildNodes() ) addSynopsisChildren( &n, level );
            --level;
            n = n.nextSibling();
        }
    }

};

PDFTocModel::PDFTocModel(Poppler::Document* document, QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roleNames;
    roleNames[Title] = "title";
    roleNames[Level] = "level";
    roleNames[PageNumber] = "pageNumber";
    setRoleNames(roleNames);

    d->document = document;
    QDomDocument* toc = document->toc();
    d->addSynopsisChildren(toc, 0);
}

PDFTocModel::~PDFTocModel()
{
    delete d;
}

QVariant PDFTocModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid())
    {
        int row = index.row();
        if(row > 0 && row < d->entries.count())
        {
            const PDFTocEntry* entry = d->entries.at(row);
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

int PDFTocModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

#include "pdftocmodel.moc"
