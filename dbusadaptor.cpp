/*
 *
 */

#include "dbusadaptor.h"

#include <QtCore/QDebug>
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
    qDebug() << Q_FUNC_INFO << files;
    if(files.count() > 0)
        QMetaObject::invokeMethod(m_view->rootObject(), "openFile", Q_ARG(QVariant, files.at(0)));
}
