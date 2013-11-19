/*
 *
 */

#include "dbusadaptor.h"

#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

DBusAdaptor::DBusAdaptor(QQuickView* view)
    : QDBusAbstractAdaptor(view), m_view{view}
{
}

DBusAdaptor::~DBusAdaptor()
{
}

void DBusAdaptor::openFile(const QStringList& files)
{
    QMetaObject::invokeMethod(m_view->rootObject(), "openFile", Q_ARG(QVariant, files.at(0)));
}
