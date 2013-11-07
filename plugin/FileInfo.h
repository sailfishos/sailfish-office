#ifndef FILEINFO_H
#define FILEINFO_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>

class FileInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY sourceChanged)
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY sourceChanged)
    Q_PROPERTY(QString mimeType READ mimeType NOTIFY sourceChanged)
    Q_PROPERTY(QString mimeTypeComment READ mimeTypeComment NOTIFY sourceChanged)
    Q_PROPERTY(QDateTime modifiedDate READ modifiedDate NOTIFY sourceChanged)

public:
    explicit FileInfo(QObject* parent = 0);
    ~FileInfo();

    QUrl source() const;
    QString fileName() const;
    qint64 fileSize() const;
    QString mimeType() const;
    QString mimeTypeComment() const;
    QDateTime modifiedDate() const;

public Q_SLOTS:
    void setSource(const QUrl& source);

Q_SIGNALS:
    void sourceChanged();

private:
    class Private;
    Private* const d;
};

#endif // FILEINFO_H
