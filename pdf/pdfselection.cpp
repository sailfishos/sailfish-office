/*
 * Copyright (C) 2015-2016 Caliste Damien.
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

#include "pdfselection.h"
#include "pdfdocument.h"

class PDFSelection::Private
{
public:
    Private()
        : canvas(nullptr)
        , pageIndexStart(-1)
        , boxIndexStart(-1)
        , pageIndexStop(-1)
        , boxIndexStop(-1)
        , handleReversed(false)
        , wiggle(4.)
    {
    }

    PDFCanvas *canvas;

    QPointF start;
    QPointF stop;

    int pageIndexStart;
    int boxIndexStart;

    int pageIndexStop;
    int boxIndexStop;

    bool handleReversed;

    qreal wiggle;

    enum Position {
        At,
        Before,
        After
    };
    void textBoxAtIndex(int index, int *pageIndex, int *boxIndex);
    void textBoxAtPoint(const QPointF &point, Position position, int *pageIndex, int *boxIndex);
    int sliceCount(int pageIndex1, int boxIndex1, int pageIndex2, int boxIndex2) const;
    int count() const;
};

PDFSelection::PDFSelection(QObject *parent)
  : QAbstractListModel(parent), d(new Private)
{
}

PDFSelection::~PDFSelection()
{
    delete d;
}

QHash<int, QByteArray> PDFSelection::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles.insert(Rect, "rect");
    roles.insert(Text, "text");

    return roles;
}

void PDFSelection::Private::textBoxAtIndex(int index, int *pageIndex, int *boxIndex)
{
    *pageIndex = -1;
    *boxIndex = -1;
    PDFDocument *doc = canvas->document();
    if (doc != nullptr) {
        int count_ = count();
        while (index < 0)
            index += count_;
        while (index >= count_)
            index -= count_;
        *boxIndex = index + boxIndexStart;
        for (*pageIndex = pageIndexStart;
             *pageIndex <= pageIndexStop; *pageIndex += 1) {
            int nBoxes = doc->textBoxesAtPage(*pageIndex).count();
            if (*boxIndex < nBoxes)
                return;
            else
              *boxIndex -= nBoxes;
        }
    }
}

QVariant PDFSelection::data(const QModelIndex& index, int role) const
{
    if (!d->canvas)
        return QVariant();
    PDFDocument *doc = d->canvas->document();
    if (!doc)
        return QVariant();

    int pageIndex = -1;
    int boxIndex;
    QVariant result;

    if (index.isValid())
        d->textBoxAtIndex(index.row(), &pageIndex, &boxIndex);
    if (pageIndex >= 0) {
        const PDFDocument::TextList &boxes = doc->textBoxesAtPage(pageIndex);
        const QPair<QRectF, Poppler::TextBox*> &box = boxes.at(boxIndex);
        switch(role) {
        case Rect:
            result.setValue<QRectF>(d->canvas->fromPageToItem(pageIndex, box.first));
            break;
        case Text:
            result.setValue<QString>(box.second->text());
            break;
        default:
            result.setValue<QString>(QString("Unknown role: %1").arg(role));
            break;
        }
    }
    return result;
}

QPair<int, QRectF> PDFSelection::rectAt(int index) const
{
    if (!d->canvas)
        return QPair<int, QRectF>();
    PDFDocument *doc = d->canvas->document();
    if (!doc)
        return QPair<int, QRectF>();

    int pageIndex, boxIndex;
    d->textBoxAtIndex(index, &pageIndex, &boxIndex);
    if (pageIndex < 0)
        return QPair<int, QRectF>();

    const PDFDocument::TextList &boxes = doc->textBoxesAtPage(pageIndex);
    const QPair<QRectF, Poppler::TextBox*> &box = boxes.at(boxIndex);
    return QPair<int, QRectF> {pageIndex, box.first};
}

int PDFSelection::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return count();
}

int PDFSelection::Private::sliceCount(int pageIndex1, int boxIndex1,
                                      int pageIndex2, int boxIndex2) const
{
    if (!canvas)
        return 0;
    PDFDocument *doc = canvas->document();
    if (!doc)
        return 0;

    if (pageIndex1 < pageIndex2 ||
        (pageIndex1 == pageIndex2 && boxIndex1 < boxIndex2)) {
        int n = -boxIndex1;
        for (int i = pageIndex1; i < pageIndex2; i++)
            n += doc->textBoxesAtPage(i).count();
        return n + boxIndex2;
    } else {
        int n = -boxIndex2;
        for (int i = pageIndex2; i < pageIndex1; i++)
            n += doc->textBoxesAtPage(i).count();
        return -n - boxIndex1;
    }
}

int PDFSelection::Private::count() const
{
    if (pageIndexStart < 0 || pageIndexStop < 0 ||
        boxIndexStart < 0 || boxIndexStop < 0) {
        return 0;
    }

    if (!canvas || !canvas->document())
        return 0;

    return sliceCount(pageIndexStart, boxIndexStart,
                      pageIndexStop, boxIndexStop) + 1;
}

int PDFSelection::count() const
{
    return d->count();
}

PDFCanvas* PDFSelection::canvas() const
{
    return d->canvas;
}

void PDFSelection::setCanvas(PDFCanvas *newCanvas)
{
    if (newCanvas != d->canvas) {
        if (d->canvas)
            d->canvas->disconnect(this);
        d->canvas = newCanvas;

        connect(d->canvas, &PDFCanvas::pageLayoutChanged,
                this, &PDFSelection::onLayoutChanged);

        emit canvasChanged();

        onLayoutChanged();
    }
}

float PDFSelection::wiggle() const
{
    return d->wiggle;
}

void PDFSelection::setWiggle(float newValue)
{
    if (newValue != d->wiggle) {
        d->wiggle = newValue;
        emit wiggleChanged();
    }
}

// TODO: update the Before and After cases to handle RTL languages.
void PDFSelection::Private::textBoxAtPoint(const QPointF &point, Position position,
                                           int *pageIndex, int *boxIndex)
{
    *pageIndex = -1;
    *boxIndex  = -1;
    if (!canvas || !canvas->document())
        return;
    
    // point is given in canvas coordinates.
    QPair<int, QRectF> at = canvas->pageAtPoint(point);
    if (at.first < 0)
        return;
    *pageIndex = at.first;

    const PDFDocument::TextList &boxes = canvas->document()->textBoxesAtPage(*pageIndex);
    switch (position) {
    case PDFSelection::Private::At: {
        qreal squaredDistanceMin = wiggle * wiggle;
        int i = 0;
        for (PDFDocument::TextList::const_iterator box = boxes.begin();
             box != boxes.end(); box++) {
            qreal squaredDistance =
                canvas->squaredDistanceFromRect(at.second, box->first, point);
                
            if (squaredDistance < squaredDistanceMin) {
                *boxIndex = i;
                squaredDistanceMin = squaredDistance;
            }
            i += 1;
        }
        break;
    }
    case PDFSelection::Private::Before:
        // Find the last box index in pageIndex that is before @point,
        // including at @point. If none is found in this page, returns
        // the last box of the previous page.
    case PDFSelection::Private::After: {
        // Find the first box index in pageIndex that is after @point,
        // including at @point. If none is found in this page, returns
        // the first box of the next page.
        QPointF reducedCoordPoint {point.x() / at.second.width(),
                (point.y() - at.second.y()) / at.second.height()};
        
        int index = 0;
        PDFSelection::Private::Position previousBoxPosition = PDFSelection::Private::Before;
        QRectF previousBox {0, -1, 1, 1};
        qreal closestSquaredDistance = 2.;
        int boxAfterIndex = boxes.length();
        for (PDFDocument::TextList::const_iterator box = boxes.begin();
             box != boxes.end(); box++) {
            PDFSelection::Private::Position currentBoxPosition =
                (box->first.bottom() < reducedCoordPoint.y()
                 || (box->first.top() < reducedCoordPoint.y()
                     && box->first.left() < reducedCoordPoint.x()))
                ? PDFSelection::Private::Before
                : PDFSelection::Private::After;
            if (previousBox.contains(reducedCoordPoint))
                currentBoxPosition = PDFSelection::Private::After;
            // Find a transition in the list of boxes. A transition is
            // when the current box switch from being before the given point
            // to being after.
            if (previousBoxPosition == PDFSelection::Private::Before &&
                currentBoxPosition == PDFSelection::Private::After) {
                // The actual transition that will be kept is the closest
                // to the given point.
                QPointF boxCenter = previousBox.center();
                qreal squaredDistance =
                    (boxCenter.x() - reducedCoordPoint.x()) *
                    (boxCenter.x() - reducedCoordPoint.x()) +
                    (boxCenter.y() - reducedCoordPoint.y()) *
                    (boxCenter.y() - reducedCoordPoint.y());
                if (squaredDistance < closestSquaredDistance) {
                    boxAfterIndex = index;
                    closestSquaredDistance = squaredDistance;
                }
                boxCenter = box->first.center();
                squaredDistance =
                    (boxCenter.x() - reducedCoordPoint.x()) *
                    (boxCenter.x() - reducedCoordPoint.x()) +
                    (boxCenter.y() - reducedCoordPoint.y()) *
                    (boxCenter.y() - reducedCoordPoint.y());
                if (squaredDistance < closestSquaredDistance) {
                    boxAfterIndex = index;
                    closestSquaredDistance = squaredDistance;
                }
            }
            previousBoxPosition = currentBoxPosition;
            previousBox = box->first;
            index += 1;
        }
        // After the for loop, boxAfterIndex is in [0; boxes.length()].
        // Assign *boxIndex according to argument position and value of boxAfterIndex.
        if (position == PDFSelection::Private::Before
            || (position == PDFSelection::Private::After
                && boxAfterIndex > 0
                && boxes[boxAfterIndex - 1].first.contains(reducedCoordPoint))) {
            *boxIndex = boxAfterIndex - 1;
        } else {
            *boxIndex = boxAfterIndex;
        }
        // Adjust *pageIndex and *boxIndex for specific boundary conditions.
        if (*boxIndex < 0) {
            *pageIndex -= 1;
            if (*pageIndex >= 0)
                *boxIndex = canvas->document()->textBoxesAtPage(*pageIndex).length() - 1;
        } else if (*boxIndex == boxes.length()) {
            *pageIndex += 1;
            if (*pageIndex >= canvas->document()->pageCount())
                *pageIndex = -1;
            *boxIndex = 0;
        }
        break;
    }
    default:
        break;
    }
}

void PDFSelection::setStart(const QPointF &point)
{
    int pageIndex, boxIndex;
    bool swap;

    d->textBoxAtPoint(point, PDFSelection::Private::After, &pageIndex, &boxIndex);
    if (pageIndex < 0 || boxIndex < 0)
      return;

    swap = (pageIndex > d->pageIndexStop
            || (pageIndex == d->pageIndexStop && boxIndex > d->boxIndexStop));
    if (swap) {
        pageIndex = d->pageIndexStop;
        boxIndex = d->boxIndexStop;
        d->handleReversed = !d->handleReversed;
        setStop(point);
        if (pageIndex == d->pageIndexStop && boxIndex == d->boxIndexStop)
            emit handle1Changed();
    }
    if (pageIndex == d->pageIndexStart && boxIndex == d->boxIndexStart) {
        if (swap)
            emit handle2Changed();
        return;
    }

    const PDFDocument::TextList &boxes = d->canvas->document()->textBoxesAtPage(pageIndex);
    QRectF box = boxes[boxIndex].first;

    int nBoxes = d->sliceCount(pageIndex, boxIndex, d->pageIndexStart, d->boxIndexStart);
    if (nBoxes > 0) {
        beginInsertRows(QModelIndex(), 0, nBoxes - 1);
        d->pageIndexStart = pageIndex;
        d->boxIndexStart = boxIndex;
        endInsertRows();
    } else if (nBoxes < 0) {
        beginRemoveRows(QModelIndex(), 0, -nBoxes - 1);
        d->pageIndexStart = pageIndex;
        d->boxIndexStart = boxIndex;
        endRemoveRows();
    }

    d->start = QPointF(box.x(), box.y() + box.height() / 2.);
    if (d->handleReversed)
        emit handle2Changed();
    else
        emit handle1Changed();

    emit countChanged();
    emit textChanged();
}

QPointF PDFSelection::handle1() const
{
    if (!d->canvas ||
        ((d->handleReversed) ? d->pageIndexStop : d->pageIndexStart) < 0) {
        return QPointF();
    }
    
    if (d->handleReversed)
        return d->canvas->fromPageToItem(d->pageIndexStop, d->stop);
    else
        return d->canvas->fromPageToItem(d->pageIndexStart, d->start);
}

float PDFSelection::handle1Height() const
{
    if (d->handleReversed)
        return d->canvas->fromPageToItem(d->pageIndexStop, rectAt(-1).second).height();
    else
        return d->canvas->fromPageToItem(d->pageIndexStart, rectAt(0).second).height();
}

void PDFSelection::setHandle1(const QPointF &point)
{
    if (d->handleReversed)
        setStop(point);
    else
        setStart(point);
}

void PDFSelection::setStop(const QPointF &point)
{
    int pageIndex, boxIndex;
    bool swap;
  
    d->textBoxAtPoint(point, PDFSelection::Private::Before, &pageIndex, &boxIndex);
    if (pageIndex < 0 || boxIndex < 0)
        return;

    swap = (pageIndex < d->pageIndexStart
            || (pageIndex == d->pageIndexStart && boxIndex < d->boxIndexStart));
    if (swap) {
        pageIndex = d->pageIndexStart;
        boxIndex = d->boxIndexStart;
        d->handleReversed = !d->handleReversed;
        setStart(point);
        if (pageIndex == d->pageIndexStart && boxIndex == d->boxIndexStart)
            emit handle2Changed();
    }
    if (pageIndex == d->pageIndexStop && boxIndex == d->boxIndexStop) {
        if (swap)
            emit handle1Changed();
        return;
    }

    const PDFDocument::TextList &boxes = d->canvas->document()->textBoxesAtPage(pageIndex);
    QRectF box = boxes[boxIndex].first;

    int count_ = count(); 
    int nBoxes = d->sliceCount(d->pageIndexStop, d->boxIndexStop, pageIndex, boxIndex);
    if (nBoxes > 0) {
        beginInsertRows(QModelIndex(), count_, count_ + nBoxes - 1);
        d->pageIndexStop = pageIndex;
        d->boxIndexStop = boxIndex;
        endInsertRows();
    } else if (nBoxes < 0) {
        beginRemoveRows(QModelIndex(), count_ + nBoxes, count_ - 1);
        d->pageIndexStop = pageIndex;
        d->boxIndexStop = boxIndex;
        endRemoveRows();
    }

    d->stop = QPointF(box.x() + box.width(), box.y() + box.height() / 2.);
    if (d->handleReversed)
        emit handle1Changed();
    else
        emit handle2Changed();

    emit countChanged();
    emit textChanged();
}

QPointF PDFSelection::handle2() const
{
    if (!d->canvas ||
        ((d->handleReversed) ? d->pageIndexStart : d->pageIndexStop) < 0) {
        return QPointF();
    }

    if (d->handleReversed)
        return d->canvas->fromPageToItem(d->pageIndexStart, d->start);
    else
        return d->canvas->fromPageToItem(d->pageIndexStop, d->stop);
}

float PDFSelection::handle2Height() const
{
    if (d->handleReversed)
        return d->canvas->fromPageToItem(d->pageIndexStart, rectAt(0).second).height();
    else
        return d->canvas->fromPageToItem(d->pageIndexStop, rectAt(-1).second).height();
}

void PDFSelection::setHandle2(const QPointF &point)
{
    if (d->handleReversed)
        setStart(point);
    else
        setStop(point);
}

bool PDFSelection::selectAt(const QPointF &point)
{
    if (d->pageIndexStart >= 0 && d->boxIndexStart >= 0
        && d->pageIndexStop >= 0 && d->boxIndexStop >= 0) {
        unselect();
    }

    int pageIndex, boxIndex;
    d->textBoxAtPoint(point, PDFSelection::Private::At, &pageIndex, &boxIndex);
    if (pageIndex < 0 || boxIndex < 0)
        return false;

    const PDFDocument::TextList &boxes = d->canvas->document()->textBoxesAtPage(pageIndex);
    QRectF box = boxes[boxIndex].first;

    beginInsertRows(QModelIndex(), 0, 0);
    d->pageIndexStart = pageIndex;
    d->boxIndexStart = boxIndex;
    d->pageIndexStop = pageIndex;
    d->boxIndexStop = boxIndex;
    endInsertRows();

    d->handleReversed = false;

    d->start = QPointF(box.x(), box.y() + box.height() / 2.);
    emit handle1Changed();
    d->stop = QPointF(box.x() + box.width(), box.y() + box.height() / 2.);
    emit handle2Changed();

    emit countChanged();
    emit textChanged();

    return true;
}

void PDFSelection::unselect()
{
    beginResetModel();
    d->pageIndexStart = d->pageIndexStop = -1;
    d->boxIndexStart = d->boxIndexStop = -1;
    endResetModel();
    emit countChanged();
    emit textChanged();
}

void PDFSelection::onLayoutChanged()
{
    int nselection = count();
    if (nselection == 0)
        return;

    emit handle1Changed();
    emit handle2Changed();
    emit dataChanged(createIndex(0, 0), createIndex(nselection - 1, 0),
                     QVector<int>{Rect});
}

bool PDFSelection::selectionAtPoint(const QPointF &point) const
{
    if (d->pageIndexStart < 0 || d->pageIndexStop < 0
        || d->boxIndexStart < 0 || d->boxIndexStop < 0) {
        return false;
    }

    if (!d->canvas)
        return false;
    PDFDocument *doc = d->canvas->document();
    if (!doc)
        return false;
    
    QPair<int, QRectF> at = d->canvas->pageAtPoint(point);
    if (at.first < 0)
        return false;
    QPointF reducedCoordPoint {point.x() / at.second.width(),
            (point.y() - at.second.y()) / at.second.height()};
    const PDFDocument::TextList &boxes = doc->textBoxesAtPage(at.first);
    for (int i = ((at.first == d->pageIndexStart) ? d->boxIndexStart : 0);
         i < ((at.first == d->pageIndexStop) ? d->boxIndexStop + 1 : boxes.length());
         i++) {
        if (boxes.value(i).first.contains(reducedCoordPoint))
            return true;
    }
    return false;
}

QString PDFSelection::text() const
{
    QString out;

    if (d->pageIndexStart < 0 || d->pageIndexStop < 0 ||
        d->boxIndexStart < 0 || d->boxIndexStop < 0) {
        return out;
    }

    if (!d->canvas)
        return out;
    PDFDocument *doc = d->canvas->document();
    if (!doc)
        return out;
    
    int i, j;
    for (i = d->pageIndexStart; i <= d->pageIndexStop; i++) {
        const PDFDocument::TextList &boxes = doc->textBoxesAtPage(i);
        for (j = ((i == d->pageIndexStart) ? d->boxIndexStart : 0);
             j < ((i == d->pageIndexStop) ? d->boxIndexStop : boxes.length());
             j++) {
            Poppler::TextBox *tbox = boxes.value(j).second;
            out += tbox->text() + (tbox->hasSpaceAfter() ? " " : "");
        }
    }
    const PDFDocument::TextList &boxes = doc->textBoxesAtPage(d->pageIndexStop);
    out += boxes.value(d->boxIndexStop).second->text();
    return out;
}
