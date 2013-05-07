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

    void load( const QString& file );
    void requestPage( int index, uint width );

    static PDFRenderThread* instance();

Q_SIGNALS:
    void loadFinished();
    void pageFinished(int index, QImage image);

private Q_SLOTS:
    void processQueue();

private:
    class Private;
    Private * const d;

    static PDFRenderThread* sm_instance;
};

#endif // PDFRENDERTHREAD_H
