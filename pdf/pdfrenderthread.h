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

Q_SIGNALS:
    void loadFinished();
    void jobFinished(PDFJob* job);

private Q_SLOTS:
    void processQueue();

private:
    class Private;
    Private * const d;
};

#endif // PDFRENDERTHREAD_H
