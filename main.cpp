#include "mainwindow.h"

#include <QApplication>
#include <ctime>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
