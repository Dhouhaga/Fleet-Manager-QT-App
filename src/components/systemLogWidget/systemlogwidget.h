#ifndef SYSTEMLOGWIDGET_H
#define SYSTEMLOGWIDGET_H

#include <QWidget>

namespace Ui {
class SystemLogWidget;
}

class SystemLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemLogWidget(QWidget *parent = nullptr);
    ~SystemLogWidget();

    void logMessage(const QString& message);
    void on_clearLogButton_clicked();

private:
    Ui::SystemLogWidget *ui;
};

#endif
