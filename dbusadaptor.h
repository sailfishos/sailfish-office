#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>

class QQuickView;
class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.sailfish.office.ui")

public:
    DBusAdaptor(QQuickView* view);
    ~DBusAdaptor();

public Q_SLOTS:
    Q_NOREPLY void openFile(const QStringList& files);

private:
    QQuickView* m_view;
};

#endif // DBUSADAPTOR_H
