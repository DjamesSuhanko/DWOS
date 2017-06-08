#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.installTranslator(&w.translator);
    w.show();

    return a.exec();
}
