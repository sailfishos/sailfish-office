#ifndef DOCUMENTPROVIDERLISTMODEL_H
#define DOCUMENTPROVIDERLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlListProperty>
#include </usr/include/qt5/QtQml/QQmlParserStatus>

QT_BEGIN_NAMESPACE
class QQmlComponent;
QT_END_NAMESPACE

class DocumentProviderPlugin;
class DocumentProviderListModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<DocumentProviderPlugin> sources READ sources)
    Q_PROPERTY(int count READ rowCount NOTIFY sourcesChanged)
    Q_PROPERTY(QQmlComponent *albumDelegate READ albumDelegate WRITE setAlbumDelegate NOTIFY albumDelegateChanged)
    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "sources")
public:
    enum ProviderRoles {
        Title = Qt::UserRole + 1,
        Description,
        Icon,
        Count,
        Thumbnail,
        Page,
        Ready,
        ProviderModel
    };

    explicit DocumentProviderListModel(QObject* parent = 0);
    virtual ~DocumentProviderListModel();

    virtual void classBegin();
    virtual void componentComplete();

    QQmlListProperty<DocumentProviderPlugin> sources();

    QQmlComponent *albumDelegate() const;
    void setAlbumDelegate(QQmlComponent *albumDelegate);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QHash< int, QByteArray > roleNames() const;

Q_SIGNALS:
    void sourcesChanged();
    void albumDelegateChanged();

private slots:
    void updateActiveSources();
    void sourceInfoChanged();

private:
    class Private;
    Private* d;
};

#endif // DOCUMENTPROVIDERLISTMODEL_H
