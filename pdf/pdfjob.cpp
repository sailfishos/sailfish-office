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
}

RenderPageJob::RenderPageJob(int index, uint width, Poppler::Document* document)
    : PDFJob{ PDFJob::RenderPageJob }, m_index{ index }, m_width{ width }, m_document{ document }
{

}

void RenderPageJob::run()
{
    Poppler::Page* page = m_document->page( m_index );
    m_page = page->renderToImage().scaledToWidth( m_width, Qt::SmoothTransformation );
}
