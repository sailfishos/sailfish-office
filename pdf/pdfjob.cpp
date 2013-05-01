/*
 *
 */

#include "pdfjob.h"
#include <poppler-qt4.h>

LoadDocumentJob::LoadDocumentJob(const QString& source)
    : PDFJob{ PDFJob::LoadDocumentJob }, m_source{ source }
{

}

void LoadDocumentJob::run()
{
    m_document = Poppler::Document::load(m_source);
    m_document->setRenderHint( Poppler::Document::Antialiasing, true );
    m_document->setRenderHint( Poppler::Document::TextAntialiasing, true );
}

RenderPageJob::RenderPageJob(int index, uint width, Poppler::Document* document)
    : PDFJob{ PDFJob::RenderPageJob }, m_index{ index }, m_width{ width }, m_document{ document }
{

}

void RenderPageJob::run()
{
    Poppler::Page* page = m_document->page( m_index );
    float scale = 72.0f * ( float(m_width) / page->pageSizeF().width() );
    m_page = page->renderToImage( scale, scale );
}
