#include <QApplication>
#include <QStyleFactory.h>
#include <QDebug>

#include "MainWindow.h"


int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Windows");
    MainWindow mw;
    mw.show();
    qDebug() << "Available styles:" << QStyleFactory::keys();
    return QApplication::exec();
}
