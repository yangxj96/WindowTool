#include <QApplication>
#include <QStyleFactory.h>
#include <QDebug>

#include "WinTool/helper/ConfigManager.h"
#include "WinTool/ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Windows");
    MainWindow mw;
    mw.show();
    qDebug() << "包含主题:" << QStyleFactory::keys();
    qDebug() << "QT版本:" << qVersion();
    return QApplication::exec();
}
