
#ifndef SAILFISHAPPLICATION_H
#define SAILFISHAPPLICATION_H

#include <QSharedPointer>

class QString;
class QApplication;
class QQuickView;

namespace Sailfish 
{
    QSharedPointer< QApplication > createApplication(int& argc, char** argv);
    QSharedPointer< QQuickView > createView(const QString& file);
    void showView(const QSharedPointer<QQuickView> &view);
}

#endif // SAILFISHAPPLICATION_H

