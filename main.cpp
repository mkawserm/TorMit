#include "tmmainwindow.h"
#include "tmcontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QLatin1String("TorMit"));
    a.setApplicationVersion(QLatin1String("1.0.0"));

    TMMainWindow w;

    TMController controller;
    controller.setMainWindow(&w);
    controller.start();

    w.show();
    return a.exec();
}
