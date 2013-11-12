/*
 *
 */

#ifndef PDFRENDERTHREAD_H
#define PDFRENDERTHREAD_H

#include <QObject>
#include <QImage>

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
    QObjectList linkTargets() const;

    void queueJob( PDFJob* job );

    void setCanvasWidth( uint width );

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
