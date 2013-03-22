#ifndef DOCUMENTPROVIDERLISTMODEL_H
#define DOCUMENTPROVIDERLISTMODEL_H

#include <QAbstractListModel>
#include <QDeclarativeListProperty>
#include <QDeclarativeParserStatus>

QT_BEGIN_NAMESPACE
class QDeclarativeComponent;
QT_END_NAMESPACE

class DocumentProviderPlugin;
class DocumentProviderListModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<DocumentProviderPlugin> sources READ sources)
    Q_PROPERTY(int count READ rowCount NOTIFY sourcesChanged)
    Q_PROPERTY(QDeclarativeComponent *albumDelegate READ albumDelegate WRITE setAlbumDelegate NOTIFY albumDelegateChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
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

    QDeclarativeListProperty<DocumentProviderPlugin> sources();

    QDeclarativeComponent *albumDelegate() const;
    void setAlbumDelegate(QDeclarativeComponent *albumDelegate);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

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
