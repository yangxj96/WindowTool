#include <QApplication>

#include "MainWindow.h"


int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    MainWindow mw;
    mw.show();
    return QApplication::exec();
}
