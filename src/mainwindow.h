#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtablewidget.h>
#include "core/FleetManager.h"
#include "components/packageOperations/PackageOperations.h"
#include "components/statisticsPanel/StatisticsPanel.h"
#include "components/systemLogWidget/SystemLogWidget.h"
#include "components/vehicleCreator/VehicleCreator.h"
#include "components/vehicleInfoViewer/VehicleInfoViewer.h"
#include "components/packageCreator/PackageCreator.h"
#include "components/vehicleOperations/VehicleOperations.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Vehicle Management
    void on_createDroneButton_clicked();
    void on_createVanButton_clicked();
    void on_createRobotButton_clicked();
    void on_removeVehicleButton_clicked();

    // Package Management
    void on_createStandardPackageButton_clicked();
    void on_createFragilePackageButton_clicked();
    void on_createRefrigeratedPackageButton_clicked();
    void on_createExpressPackageButton_clicked();

    // Assignment & Delivery
    void on_assignPackageButton_clicked();
    void on_removePackageButton_clicked();
    void on_deliverButton_clicked();

    // Vehicle State
    void on_setStateButton_clicked();

    // Display
    void on_refreshButton_clicked();
    void on_showVehicleDetailsButton_clicked();
    void on_showCompatibilityButton_clicked();

private:
    Ui::MainWindow *ui;
    FleetManager mFleetManager;
    SystemLogWidget *logWidget;
    StatisticsPanel *statisticsPanel;
    VehicleInfoViewer* vehicleInfoViewer;
    PackageCreator* packageCreator;
    VehicleCreator* vehicleCreator;
    VehicleOperations* vehicleOperations;
    PackageOperations* packageOperations;

    void refreshVehicleTable();
    void refreshPackageTable();
    QTableWidgetItem* createIndexItem(int index);
    void logMessage(const QString& message);

    bool isValidVehicleName(const QString& name);
    QString formatVehicleDetails(const Vehicle* vehicle) const;
    QString formatCompatibilityInfo(int vehicleIndex, const Vehicle* vehicle) const;
    void handlePackageCreation(const QString& packageType,
                               std::function<std::unique_ptr<Package>()> createFunction);
};

#endif // MAINWINDOW_H
