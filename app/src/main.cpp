#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName("Kanako Voice Changer");
    QApplication::setOrganizationName("Kanako");
    MainWindow window;
    window.show();
    return application.exec();
}
