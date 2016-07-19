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

#include "pdfjob.h"

#include <QtMath>
#include <poppler-qt5.h>

LoadDocumentJob::LoadDocumentJob(const QString &source)
    : PDFJob(PDFJob::LoadDocumentJob), m_source(source)
{
}

void LoadDocumentJob::run()
{
    m_document = Poppler::Document::load(m_source);
    if (m_document) {
        m_document->setRenderHint(Poppler::Document::Antialiasing, true);
        m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
    }
}

UnLockDocumentJob::UnLockDocumentJob(const QString &password)
    : PDFJob(PDFJob::UnLockDocumentJob), m_password(password)
{
}

void UnLockDocumentJob::run()
{
    Q_ASSERT(m_document);

    if (m_document->isLocked())
        m_document->unlock(m_password.toUtf8(), m_password.toUtf8());
}

RenderPageJob::RenderPageJob(int index, uint width, QQuickWindow *window, QRect subpart)
    : PDFJob(PDFJob::RenderPageJob), m_index(index), m_subpart(subpart), m_page(0), m_window(window), m_width(width)
{
}

void RenderPageJob::run()
{
    Q_ASSERT(m_document);

    Poppler::Page *page = m_document->page(m_index);
    QSizeF size = page->pageSizeF();
    float scale = 72.0f * (float(m_width) / size.width());

    QImage image;
    if (m_subpart.isEmpty()) {
        image = page->renderToImage(scale, scale);
        m_subpart.setCoords(0, 0, image.width(), image.height());
    } else {
        QRect pageRect = {0, 0, (int)m_width, qCeil(size.height() / size.width() * m_width)};
        m_subpart = m_subpart.intersected(pageRect);

        image = page->renderToImage(scale, scale, m_subpart.x(), m_subpart.y(),
                                    m_subpart.width(), m_subpart.height());
    }
    // Note: assuming there's exactly one handler (PDFCanvas) to catch ownership of this when PDFDocument emits a signal with this
    m_page = m_window->createTextureFromImage(image);
    delete page;
}

void PageSizesJob::run()
{
    Q_ASSERT(m_document);

    for (int i = 0; i < m_document->numPages(); ++i) {
        Poppler::Page *page = m_document->page(i);
        m_pageSizes.append(page->pageSizeF());
        delete page;
    }
}
