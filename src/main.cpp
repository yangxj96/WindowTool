#include <QApplication>
#include <QStyleFactory.h>
#include <QDebug>

#include "MainWindow.h"
#include "ConfigManager.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Windows");
    MainWindow mw;
    mw.show();
    qDebug() << "包含主题:" << QStyleFactory::keys();
    qDebug() << "QT版本:" << qVersion();
    return QApplication::exec();
}
