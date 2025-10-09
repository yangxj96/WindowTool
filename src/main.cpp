#include <QApplication>
#include <QStyleFactory.h>
#include <QDebug>

#include "WinTool/helper/config_manager.h"
#include "WinTool/ui/main_window.h"
#include "WinTool/helper/navicat_cleanup.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("Windows");
    MainWindow mw;
    mw.show();
    qDebug() << "包含主题:" << QStyleFactory::keys();
    qDebug() << "QT版本:" << qVersion();
    NavicatCleanup::cleanup();
    return QApplication::exec();
}
