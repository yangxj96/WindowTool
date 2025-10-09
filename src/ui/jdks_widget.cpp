#include "WinTool/ui/jdks_widget.h"
#include "ui_jdks_widget.h"

JdksWidget::JdksWidget(QWidget* parent) : QWidget(parent), ui(new Ui::JdksWidget) {
    ui->setupUi(this);
}

JdksWidget::~JdksWidget() {
    delete ui;
}