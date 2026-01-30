#include "SystemLogWidget.h"
#include "ui_SystemLogWidget.h"
#include <QMessageBox>
#include <QDateTime>

SystemLogWidget::SystemLogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemLogWidget)
{
    ui->setupUi(this);

    // Connect clear log button
    connect(ui->clearLogButton, &QPushButton::clicked,
            this, &SystemLogWidget::on_clearLogButton_clicked);
}

SystemLogWidget::~SystemLogWidget()
{
    delete ui;
}

void SystemLogWidget::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logTextEdit->append("[" + timestamp + "] " + message);
}

void SystemLogWidget::on_clearLogButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear Log",
                                  "Are you sure you want to clear the system log?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ui->logTextEdit->clear();
        logMessage("Log cleared");
    }
}
