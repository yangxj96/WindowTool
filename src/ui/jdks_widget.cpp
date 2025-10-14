#include "WinTool/ui/jdks_widget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

#include "ui_jdks_widget.h"
#include "WinTool/helper/config_manager.h"

JdksWidget::JdksWidget(QWidget* parent) : QWidget(parent), ui(new Ui::JdksWidget), m_config(&ConfigManager::instance()) {
    ui->setupUi(this);
}

JdksWidget::~JdksWidget() {
    delete ui;
}

void JdksWidget::showEvent(QShowEvent* event) {
    if (!m_loaded) {
        jdks_prefix = m_config->get<QString>("jdks_prefix", "");
        // 获取前缀
        ui->le_jdk_prefix->setText(jdks_prefix);
        // 设置表头
        ui->tb_jdks->setHorizontalHeaderLabels({"JDK路径", "是否选用"});
        this->setTableData();

        m_loaded = true;
    }
    QWidget::showEvent(event);
}

void JdksWidget::setTableData() const {
    if (jdks_prefix.trimmed().isEmpty()) {
        qDebug() << "前缀为空，跳过扫描：" << jdks_prefix;
        return;
    }

    const QDir dir(jdks_prefix);
    if (!dir.exists()) {
        qWarning() << "目录不存在:" << jdks_prefix;
        return;
    }

    if (!dir.isReadable()) {
        qWarning() << "目录不可读:" << jdks_prefix;
        return;
    }

    // 获取所有子条目（只取目录，排除 . 和 ..）
    QFileInfoList entryList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    QStringList result;
    for (const QFileInfo&info: entryList) {
        if (info.isDir() && info.absoluteFilePath().startsWith(jdks_prefix)) {
            result.append(info.absoluteFilePath()); // 只取文件夹名
            // 如果需要完整路径：result.append(info.absoluteFilePath());
        }
    }
}

void JdksWidget::on_btn_jdk_prefix_pick_clicked() {
    // 弹出选择文件夹对话框
    const QString dirPath = QFileDialog::getExistingDirectory(
        this, // 父窗口
        "请选择JDK文件夹", // 对话框标题
        QDir::homePath(), // 默认打开路径（例如：用户主目录）
        QFileDialog::ShowDirsOnly // 只显示文件夹，不显示文件
    );

    if (!dirPath.isEmpty()) {
        qDebug() << "选中的文件夹：" << dirPath;
        // 可以设置到 QLineEdit 显示
        ui->le_jdk_prefix->setText(dirPath);
        m_config->set("jdks_prefix", dirPath);
    }
    else {
        qDebug() << "用户取消选择";
    }
}
