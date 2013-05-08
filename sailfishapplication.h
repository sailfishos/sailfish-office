
#ifndef SAILFISHAPPLICATION_H
#define SAILFISHAPPLICATION_H

#include <QSharedPointer>

class QString;
class QApplication;
class QDeclarativeView;

namespace Sailfish 
{
    QSharedPointer<QApplication> createApplication(int& argc, char** argv);
    QSharedPointer<QDeclarativeView> createView(const QString& file);
    void showView(const QSharedPointer<QDeclarativeView> &view);
}

#endif // SAILFISHAPPLICATION_H

