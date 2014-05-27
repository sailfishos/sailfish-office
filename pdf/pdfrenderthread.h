/*
 *
 */

#ifndef PDFRENDERTHREAD_H
#define PDFRENDERTHREAD_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QMultiMap>
#include <QtGui/QImage>

class QSize;
class PDFJob;
class PDFRenderThreadPrivate;

class PDFRenderThread : public QObject
{
    Q_OBJECT
public:
    PDFRenderThread( QObject* parent = 0 );
    ~PDFRenderThread();

    int pageCount() const;
    QObject* tocModel() const;
    bool isLoaded() const;
    QMultiMap< int, QPair< QRectF, QUrl > > linkTargets() const;

    void queueJob( PDFJob* job );
    void cancelRenderJob(int index);
    void prioritizeJob(int index, int size);

Q_SIGNALS:
    void loadFinished();
    void jobFinished(PDFJob* job);

private:
    friend class PDFRenderThreadPrivate;

    PDFRenderThreadPrivate * const d;
};

#endif // PDFRENDERTHREAD_H
