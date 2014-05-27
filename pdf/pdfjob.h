/*
 *
 */

#ifndef PDFJOB_H
#define PDFJOB_H

#include <QString>
#include <QImage>
#include <QObject>
#include <QtQuick/QQuickWindow>

namespace Poppler
{
    class Document;
}

class PDFJob : public QObject
{
    Q_OBJECT
public:
    enum JobType {
        LoadDocumentJob,
        RenderPageJob,
        PageSizesJob,
    };

    PDFJob( JobType type ) : m_document{ nullptr }, m_type{ type } { }
    virtual ~PDFJob() { }

    virtual void run() = 0;

    JobType type() const { return m_type; }

protected:
    friend class PDFRenderThreadPrivate;
    Poppler::Document* m_document;

private:
    JobType m_type;
};

class LoadDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    LoadDocumentJob( const QString& source );

    virtual void run();

private:
    QString m_source;
};

class RenderPageJob : public PDFJob
{
    Q_OBJECT
public:
    RenderPageJob(int index, uint width, QQuickWindow *window);

    virtual void run();

    int m_index;
    QSGTexture *m_page;

    int renderWidth() const { return m_width; }

private:
    QQuickWindow *m_window;
    uint m_width;
};

class PageSizesJob : public PDFJob
{
    Q_OBJECT
public:
    PageSizesJob() : PDFJob{ PDFJob::PageSizesJob } { }

    virtual void run();

    QList< QSizeF > m_pageSizes;
};

#endif // PDFJOB_H
