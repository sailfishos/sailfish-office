/*
 *
 */

#ifndef PDFJOB_H
#define PDFJOB_H

#include <QString>
#include <QImage>
#include <QObject>

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
    };

    PDFJob( JobType type ) : m_type{ type } { }
    virtual ~PDFJob() { }

    virtual void run() = 0;

    JobType type() const { return m_type; }

private:
    JobType m_type;
};

class LoadDocumentJob : public PDFJob
{
    Q_OBJECT
public:
    LoadDocumentJob( const QString& source );

    virtual void run();

    Poppler::Document* m_document;

private:
    QString m_source;
};

class RenderPageJob : public PDFJob
{
    Q_OBJECT
public:
    RenderPageJob(int index, uint width, Poppler::Document* document);

    virtual void run();

    int m_index;
    QImage m_page;

private:
    uint m_width;
    Poppler::Document* m_document;
};

#endif // PDFJOB_H
