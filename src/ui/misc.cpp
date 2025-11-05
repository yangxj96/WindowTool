#include "WinTool/ui/misc.h"
#include "ui_misc.h"

#include <QMessageBox>
#include "WinTool/helper/navicat_cleanup.h"

Misc::Misc(QWidget* parent) : QWidget(parent), ui(new Ui::misc) {
    ui->setupUi(this);
}

Misc::~Misc() {
    delete ui;
}

void Misc::on_btn_navicat_cleanup_clicked() {
    if (NavicatCleanup cleanup; cleanup.cleanup()) {
        QMessageBox::information(nullptr, "操作结果", "Navicat注册表清理完成！");
    } else {
        QMessageBox::warning(nullptr, "操作结果", "Navicat注册表清理完成，但部分项处理失败！\n请检查是否有足够权限。");
    }
}
