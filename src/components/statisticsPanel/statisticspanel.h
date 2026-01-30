#ifndef STATISTICSPANEL_H
#define STATISTICSPANEL_H

#include <QWidget>
#include "../../core/FleetManager.h"
namespace Ui {
class StatisticsPanel;
}

class StatisticsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsPanel(QWidget *parent = nullptr);
    ~StatisticsPanel();

    void updateStatistics(const FleetManager& fleetManager);
private:
    Ui::StatisticsPanel *ui;
};

#endif
