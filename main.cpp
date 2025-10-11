#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QGuiApplication::setDesktopFileName("nextblock"); /// fixing app icon for Wayland

    w.show();
    return a.exec();
}
