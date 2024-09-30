#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QDebug>
#include <QByteArray>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
  QFont font("MS Serif", 10);

    return a.exec();
}
