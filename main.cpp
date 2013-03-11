 
#include <QApplication>
#include <QDeclarativeView>

#include "sailfishapplication.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    auto app = Sailfish::createApplication(argc, argv);
    auto view = Sailfish::createView("main.qml");
    
    Sailfish::showView(view);
    
    app->exec();
    
    delete app;
    delete view;
    
    return 0;
}
