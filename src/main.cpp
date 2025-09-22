#include <QApplication>
#include <QFile>
#include <QtDebug>

#include "MainWindow.h"


int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QFile file(":/QtTheme/theme/Flat/Light/Blue/Pink.qss");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(file.readAll());

    MainWindow mw;
    mw.show();

    return QApplication::exec();
}
