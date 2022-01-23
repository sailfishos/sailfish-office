/*
 * Copyright (C) 2013-2014 Jolla Ltd.
 * Copyright (C) 2022 Yura Beznos <yura.beznos@you-ra.info>
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

#include "djvujob.h"

#include <QtMath>
#include <QUrlQuery>


#include <QElapsedTimer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(renderTimes, "org.sailfishos.office.DJVU.timing", QtWarningMsg)

LoadDocumentJob::LoadDocumentJob(const QString &source)
    : DJVUJob(DJVUJob::LoadDocumentJob), m_source(source)
{
}

void LoadDocumentJob::run()
{
    m_document = new KDjVu();
    if (!m_document->openFile(m_source)){
        qWarning() << "Not able to load file" << m_source;
    }

}

/*
UnLockDocumentJob::UnLockDocumentJob(const QString &password)
    : DJVUJob(DJVUJob::UnLockDocumentJob), m_password(password)
{
}

void UnLockDocumentJob::run()
{
    Q_ASSERT(m_document);

    if (m_document->isLocked())
        m_document->unlock(m_password.toUtf8(), m_password.toUtf8());
}

LinksJob::LinksJob(int page)
    : DJVUJob(DJVUJob::LinksJob), m_page(page)
{
}

void LinksJob::run()
{
    Q_ASSERT(m_document);

    if (m_document->isLocked() || m_page < 0 || m_page >= m_document->numPages()) {
        return;
    }

    Poppler::Page *page = m_document->page(m_page);
    QList<Poppler::Link*> links = page->links();
    for (Poppler::Link* link : links) {
        // link->linkArea() may return negative heights,
        // as mentioned in Freedesktop bug:
        // https://bugs.freedesktop.org/show_bug.cgi?id=93900
        // To avoid later unexpected asumption on height,
        // link->linkArea() is normalized.
        switch (link->linkType()) {
        case (Poppler::Link::Browse): {
            Poppler::LinkBrowse *realLink = static_cast<Poppler::LinkBrowse*>(link);
            QRectF linkArea = link->linkArea().normalized();
            m_links.append(QPair<QRectF, QUrl>(linkArea, realLink->url()));
            break;
        }
        case (Poppler::Link::Goto): {
            Poppler::LinkGoto *gotoLink = static_cast<Poppler::LinkGoto*>(link);
            // Not handling goto link to external file currently.
            if (gotoLink->isExternal())
                break;
            QRectF linkArea = link->linkArea().normalized();
            QUrl linkURL = QUrl("");
            QUrlQuery query = QUrlQuery();
            query.addQueryItem("page", QString::number(gotoLink->destination().pageNumber()));
            if (gotoLink->destination().isChangeLeft()) {
                query.addQueryItem("left", QString::number(gotoLink->destination().left()));
            }
            if (gotoLink->destination().isChangeTop()) {
                query.addQueryItem("top", QString::number(gotoLink->destination().top()));
            }
            linkURL.setQuery(query);
            m_links.append(QPair<QRectF, QUrl>(linkArea, linkURL));
            break;
        }
        default:
            break;
        }

    }

    qDeleteAll(links);
    delete page;
}
*/
RenderPageJob::RenderPageJob(int requestId, int index, uint width,
                             QRect subpart, int extraData)
    : DJVUJob(DJVUJob::RenderPageJob), m_requestId(requestId), m_index(index), m_subpart(subpart), m_page(0), m_extraData(extraData), m_width(width)
{
}

void RenderPageJob::run()
{
    Q_ASSERT(m_document);


    QElapsedTimer timer;
    timer.start();



    if(m_document->pages().isEmpty())
        return;

    const KDjVu::Page *page = m_document->pages().at(m_index);

    // from poppler Page::pageSizeF()
    QSizeF size;
    if (page->orientation() != 0){
        size.setWidth((qreal)page->height());
        size.setHeight((qreal)page->width());
    } else {
        size.setWidth((qreal)page->width());
        size.setHeight((qreal)page->height());
    }
    //qWarning() << "size.width" << size.width();
    //qWarning() << "size.height" << size.height();

    float scale = 72.0f * (float(m_width) / size.width());

    if (qRound(size.width()) == 0
            || qRound(size.height()) == 0 )
        return;

    if (m_subpart.isEmpty()) {
        //qWarning() << "x" << qRound(size.width() * scale / 72.0f);
        //qWarning() << "y" << qRound(size.height() * scale / 72.0f);

        //(w == -1 ? qRound(size.width() * xres / 72.0) : w, h == -1 ? qRound(size.height() * yres / 72.0
        m_page = m_document->image(m_index,
                                   qRound(size.width() * scale / 72.0f),
                                   qRound(size.height() * scale / 72.0f),
                                   page->orientation());//page->renderToImage(scale, scale);
        //qWarning() << "m_page.width()" << m_page.width();
        //qWarning() << "m_page.height()" << m_page.height();
        m_subpart.setCoords(0, 0, m_page.width(), m_page.height());
    } else {
        //TODO: proper zoom
        QRect pageRect = {0, 0, int(m_width), qCeil(size.height() / size.width() * m_width)};
        m_subpart = m_subpart.intersected(pageRect);

        //m_page = page->renderToImage(scale, scale, m_subpart.x(), m_subpart.y(),
        //                            m_subpart.width(), m_subpart.height());
        // (double xres, double yres, int xPos, int yPos, int w, int h, Rotation rotate,

        //QImage tmpimg(m_subpart.width() == -1 ? qRound(size.width() * scale / 72.0f) : m_subpart.width(),
        //              m_subpart.height() == -1 ? qRound(size.height() * scale / 72.0f) : m_subpart.height(),
        //              QImage::Format_ARGB32);

        //QColor bgColor(128, 128, 128, 255);

        //tmpimg.fill(bgColor);
        int w = (m_subpart.width() == -1 ? qRound(size.width() * scale / 72.0f) : m_subpart.width());
        int h = (m_subpart.height() == -1 ? qRound(size.height() * scale / 72.0f) : m_subpart.height());
        //qWarning() << "else m_subpart.x()" << m_subpart.x();
        //qWarning() << "else m_subpart.y()" << m_subpart.y();
        //qWarning() << "else w" << w;
        //qWarning() << "else h" << h;

        //if (w == 0 || h == 0)
        //    return;
        //image might be empty, and it looks like it kills renderer
        // tried to use workaround with dummy image, but without luck
        // to reproduce need to find a region which isn't loaded
        QImage _m_page = m_document->image(m_index,
                                   qRound(size.width() * scale / 72.0f),// page->width(),//m_subpart.width(),//qRound(m_subpart.width()),// * scale / 72.0),
                                   qRound(size.height() * scale / 72.0f),//page->height(),//m_subpart.height(),//qRound(m_subpart.height()),// * scale / 72.0),
                                   page->orientation()).copy(
                                           m_subpart.x(),
                                           m_subpart.y(),
                                           w,
                                           h);
        //qWarning() << "else _m_page.width()" << _m_page.width();
        //qWarning() << "else _m_page.height()" << _m_page.height();

        if (_m_page.width() == 0 || _m_page.height() == 0) {
            //dummy image
            m_page = QPixmap(size.width(), size.height()).toImage();
            return;
        }

        m_page = _m_page;

    }

    qCDebug(renderTimes)
            << "Completed a render job for the page" << m_index
            << "with the scale" << scale
            << "and the sub rect" << m_subpart
            << "in" << timer.elapsed() << "ms"
            << "to produce an image this big" << m_page.size();
/*
    delete page; */
}

void PageSizesJob::run()
{
    Q_ASSERT(m_document);

    for (int i = 0; i < m_document->pages().length(); ++i) {
        const KDjVu::Page *page = m_document->pages().at(i);
        QSizeF dimensions((qreal)page->width(), (qreal)page->height());
        m_pageSizes.append(dimensions);// page->pageSizeF());
        //delete dimensions;
        //delete page;
    }
}

