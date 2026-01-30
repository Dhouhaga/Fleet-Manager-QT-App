#include "StatisticsPanel.h"
#include "ui_StatisticsPanel.h"

StatisticsPanel::StatisticsPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsPanel)
{
    ui->setupUi(this);
}

StatisticsPanel::~StatisticsPanel()
{
    delete ui;
}

void StatisticsPanel::updateStatistics(const FleetManager& fleetManager)
{
    int vehicleCount = fleetManager.getVehicleCount();
    double totalCapacity = fleetManager.getTotalFleetCapacity();
    double usedCapacity = fleetManager.getUsedCapacity();
    double availableCapacity = totalCapacity - usedCapacity;

    // Update system status
    if (vehicleCount > 0) {
        ui->systemStatusLabel->setText("Operational");
        ui->systemStatusLabel->setStyleSheet("border-style: solid; border-width: 2px; "
                                             "border-color: #27ae60; background-color: #ffffff; "
                                             "color: #27ae60; padding: 6px; border-radius: 5px; "
                                             "font-weight: bold;");
    } else {
        ui->systemStatusLabel->setText("No Vehicles");
        ui->systemStatusLabel->setStyleSheet("border-style: solid; border-width: 2px; "
                                             "border-color: #e74c3c; background-color: #ffffff; "
                                             "color: #e74c3c; padding: 6px; border-radius: 5px; "
                                             "font-weight: bold;");
    }

    // Update capacity stats
    ui->totalCapacityLabel->setText(QString("Total Fleet Capacity: %1g").arg(totalCapacity, 0, 'f', 1));
    ui->usedCapacityLabel->setText(QString("Used Capacity: %1g").arg(usedCapacity, 0, 'f', 1));
    ui->availableCapacityLabel->setText(QString("Available Capacity: %1g").arg(availableCapacity, 0, 'f', 1));
}
