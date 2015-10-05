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

#include <poppler-qt5.h>

LoadDocumentJob::LoadDocumentJob(const QString& source)
    : PDFJob{ PDFJob::LoadDocumentJob }, m_source{ source }
{

}

void LoadDocumentJob::run()
{
    m_document = Poppler::Document::load(m_source);
    if (m_document) {
        m_document->setRenderHint( Poppler::Document::Antialiasing, true );
        m_document->setRenderHint( Poppler::Document::TextAntialiasing, true );
    }
}

UnLockDocumentJob::UnLockDocumentJob(const QString& password)
    : PDFJob(PDFJob::UnLockDocumentJob), m_password(password)
{

}

void UnLockDocumentJob::run()
{
    Q_ASSERT(m_document);

    if (m_document->isLocked())
        m_document->unlock( m_password.toUtf8(), m_password.toUtf8() );
}

RenderPageJob::RenderPageJob(int index, uint width, QQuickWindow *window)
    : PDFJob{ PDFJob::RenderPageJob }, m_index{ index }, m_page{ 0 }, m_window{ window }, m_width{ width }
{
}

void RenderPageJob::run()
{
    Q_ASSERT(m_document);

    Poppler::Page* page = m_document->page( m_index );
    float scale = 72.0f * ( float(m_width) / page->pageSizeF().width() );
    QImage image = page->renderToImage( scale, scale );
    // Note: assuming there's exactly one handler (PDFCanvas) to catch ownership of this when PDFDocument emits a signal with this
    m_page = m_window->createTextureFromImage(image);
}

void PageSizesJob::run()
{
    Q_ASSERT(m_document);

    for ( int i = 0; i < m_document->numPages(); ++i )
    {
        Poppler::Page* page = m_document->page( i );
        m_pageSizes.append( page->pageSizeF() );
    }
}

SearchDocumentJob::SearchDocumentJob(const QString& search, uint page)
  : PDFJob(PDFJob::SearchDocumentJob), m_search(search), startPage(page)
{
  
}

void SearchDocumentJob::run()
{
    Q_ASSERT(m_document);

    for ( int i = 0; i < m_document->numPages(); ++i )
    {
        int ipage = (startPage + i) % m_document->numPages();
        Poppler::Page* page = m_document->page(ipage);
    
        double sLeft, sTop, sRight, sBottom;
        float scaleW = 1.f / page->pageSizeF().width();
        float scaleH = 1.f / page->pageSizeF().height();
        bool found;
        found = page->search(m_search, sLeft, sTop, sRight, sBottom,
                             Poppler::Page::FromTop,
                             Poppler::Page::CaseInsensitive);
        while (found) {
            QRectF result;
            result.setLeft(sLeft * scaleW);
            result.setTop(sTop * scaleH);
            result.setRight(sRight * scaleW);
            result.setBottom(sBottom * scaleH);
            m_matches.append(QPair<int, QRectF>(ipage, result));
            found = page->search(m_search, sLeft, sTop, sRight, sBottom,
                                 Poppler::Page::NextResult,
                                 Poppler::Page::CaseInsensitive);
        }
    }
}
