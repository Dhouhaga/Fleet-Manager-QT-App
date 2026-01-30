#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "components/statisticsPanel/StatisticsPanel.h"
#include "components/systemLogWidget/SystemLogWidget.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDateTime>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logWidget(nullptr)
    , statisticsPanel(nullptr)
{
    ui->setupUi(this);

    // Set table properties
    ui->vehicleTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->pendingPackagesTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->vehicleTableWidget->verticalHeader()->setVisible(false);
    ui->pendingPackagesTableWidget->verticalHeader()->setVisible(false);


    // Set column widths for vehicle table
    ui->vehicleTableWidget->setColumnCount(6);
    ui->vehicleTableWidget->setHorizontalHeaderLabels(QStringList()
                                                      << "Index" << "Name" << "Type" << "Load (g)" << "Available (g)" << "State");
    ui->vehicleTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->vehicleTableWidget->setColumnWidth(0, 60);    // Index
    ui->vehicleTableWidget->setColumnWidth(1, 120);   // Name
    ui->vehicleTableWidget->setColumnWidth(2, 80);    // Type
    ui->vehicleTableWidget->setColumnWidth(3, 80);    // Load
    ui->vehicleTableWidget->setColumnWidth(4, 100);   // Capacity
    ui->vehicleTableWidget->setColumnWidth(5, 120);   // State

    // Set column widths for package table
    ui->pendingPackagesTableWidget->setColumnCount(5);
    ui->pendingPackagesTableWidget->setHorizontalHeaderLabels(QStringList()
                                                              << "Index" << "Type" << "Description" << "Weight (g)" << "Details");
    ui->pendingPackagesTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->pendingPackagesTableWidget->setColumnWidth(0, 60);    // Index
    ui->pendingPackagesTableWidget->setColumnWidth(1, 100);   // Type
    ui->pendingPackagesTableWidget->setColumnWidth(2, 150);   // Description
    ui->pendingPackagesTableWidget->setColumnWidth(3, 80);    // Weight

    // stats panel widget
    statisticsPanel = new StatisticsPanel(this);
    ui->verticalLayout_system->addWidget(statisticsPanel);

    // log widget
    logWidget = new SystemLogWidget(this);
    ui->verticalLayout_log->addWidget(logWidget);

    refreshVehicleTable();
    refreshPackageTable();
    logWidget->logMessage("System started - Tables initialized");

    vehicleInfoViewer = new VehicleInfoViewer(this);
    packageCreator = new PackageCreator(this);
    vehicleCreator = new VehicleCreator(this);
    vehicleOperations = new VehicleOperations(this);
    packageOperations = new PackageOperations(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// =============================================
// Refresh methods for tables
// =============================================

QTableWidgetItem* MainWindow::createIndexItem(int index) {
    QTableWidgetItem* item = new QTableWidgetItem(QString::number(index));
    item->setData(Qt::UserRole, index);
    item->setBackground(QColor("#2c3e50"));  // Dark blue background
    item->setForeground(Qt::white);          // White text
    item->setTextAlignment(Qt::AlignCenter); // Centered text
    return item;
}

void MainWindow::refreshVehicleTable() {
    ui->vehicleTableWidget->setRowCount(0);

    const auto& vehicles = mFleetManager.getAllVehicles();

    for (size_t i = 0; i < vehicles.size(); ++i) {
        const Vehicle* vehicle = vehicles[i].get();

        ui->vehicleTableWidget->insertRow(i);

        // Store index
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i));
        indexItem->setData(Qt::UserRole, static_cast<int>(i));
        ui->vehicleTableWidget->setItem(i, 0, createIndexItem(i));

        // Use direct getters
        ui->vehicleTableWidget->setItem(i, 1, new QTableWidgetItem(
                                                  QString::fromStdString(vehicle->getName())));
        ui->vehicleTableWidget->setItem(i, 2, new QTableWidgetItem(
                                                  QString::fromStdString(vehicle->getType())));
        ui->vehicleTableWidget->setItem(i, 3, new QTableWidgetItem(
                                                  QString::number(vehicle->getCurrentLoad(), 'f', 1)));
        ui->vehicleTableWidget->setItem(i, 4, new QTableWidgetItem(
                                                  QString::number(vehicle->getMaxCapacity() - vehicle->getCurrentLoad(), 'f', 1)));

        // State with color coding
        QString state;
        VehicleState vehicleState = vehicle->getState();
        QTableWidgetItem* stateItem = new QTableWidgetItem();

        switch (vehicleState) {
        case VehicleState::IN_SERVICE:
            state = "In service";
            stateItem->setForeground(Qt::darkGreen);
            stateItem->setBackground(QColor(200, 255, 200));
            break;
        case VehicleState::OUT_OF_SERVICE:
            state = "Out of service";
            stateItem->setForeground(Qt::red);
            stateItem->setBackground(QColor(255, 200, 200));
            break;
        case VehicleState::AWAITING_DELIVERY:
            state = "Awaiting delivery";
            stateItem->setForeground(Qt::blue);
            stateItem->setBackground(QColor(200, 200, 255));
            break;
        }
        stateItem->setText(state);
        ui->vehicleTableWidget->setItem(i, 5, stateItem);
    }

    if (vehicles.empty()) {
        ui->vehicleTableWidget->insertRow(0);
        QTableWidgetItem* emptyItem = new QTableWidgetItem("No vehicles in fleet");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        ui->vehicleTableWidget->setItem(0, 0, emptyItem);
        ui->vehicleTableWidget->setSpan(0, 0, 1, 6);

    }
    ui->vehicleCountLabel->setText(QString("Vehicles: %1").arg(mFleetManager.getVehicleCount()));
    statisticsPanel->updateStatistics(mFleetManager);

}

void MainWindow::refreshPackageTable() {
    ui->pendingPackagesTableWidget->setRowCount(0);

    const auto& packages = mFleetManager.getAllPendingPackages();

    for (size_t i = 0; i < packages.size(); ++i) {
        const Package* package = packages[i].get();

        ui->pendingPackagesTableWidget->insertRow(i);

        // Store index
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i));
        indexItem->setData(Qt::UserRole, static_cast<int>(i));
        ui->pendingPackagesTableWidget->setItem(i, 0, createIndexItem(i));

        // Type with color coding
        QString type = QString::fromStdString(package->getType()).toUpper();
        QTableWidgetItem* typeItem = new QTableWidgetItem(type);

        if (type == "FRAGILE") {
            typeItem->setForeground(Qt::darkRed);
            typeItem->setBackground(QColor(255, 220, 220));
        } else if (type == "EXPRESS") {
            typeItem->setForeground(Qt::darkYellow);
            typeItem->setBackground(QColor(255, 255, 220));
        } else if (type == "REFRIGERATED") {
            typeItem->setForeground(Qt::darkBlue);
            typeItem->setBackground(QColor(220, 220, 255));
        } else if (type == "STANDARD") {
            typeItem->setForeground(Qt::darkGreen);
            typeItem->setBackground(QColor(220, 255, 220));
        }
        ui->pendingPackagesTableWidget->setItem(i, 1, typeItem);

        // Description
        ui->pendingPackagesTableWidget->setItem(i, 2, new QTableWidgetItem(
                                                          QString::fromStdString(package->getDescription())));

        // Weight
        QTableWidgetItem* weightItem = new QTableWidgetItem(
            QString::number(package->getWeight(), 'f', 1));
        weightItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pendingPackagesTableWidget->setItem(i, 3, weightItem);

        // Details - needed to cast to specific types
        QString details;
        if (const FragilePackage* fragile = dynamic_cast<const FragilePackage*>(package)) {
            details = "Fragility: " + QString::fromStdString(
                          FragilePackage::fragilityToString(fragile->getFragility()));
        } else if (const RefrigeratedPackage* refrigerated = dynamic_cast<const RefrigeratedPackage*>(package)) {
            details = "Temperature: " + QString::number(refrigerated->getTemperature()) + "°C";
        } else if (dynamic_cast<const ExpressPackage*>(package)) {
            details = "URGENT";
        }
        ui->pendingPackagesTableWidget->setItem(i, 4, new QTableWidgetItem(details));
    }

    if (packages.empty()) {
        ui->pendingPackagesTableWidget->insertRow(0);
        QTableWidgetItem* emptyItem = new QTableWidgetItem("No pending packages");
        emptyItem->setTextAlignment(Qt::AlignCenter);
        ui->pendingPackagesTableWidget->setItem(0, 0, emptyItem);
        ui->pendingPackagesTableWidget->setSpan(0, 0, 1, 5);
    }
    ui->packageCountLabel->setText(QString("Pending Packages: %1").arg(mFleetManager.getPendingPackageCount()));
    statisticsPanel->updateStatistics(mFleetManager);

}
// =============================================
// Vehicle Management
// =============================================

void MainWindow::on_createDroneButton_clicked()
{
    auto result = vehicleCreator->createDrone();
    QString name = result.first;
    double autonomy = result.second;

    if (!name.isEmpty()) {
        mFleetManager.createDrone(name.toStdString(), autonomy);
        refreshVehicleTable();
         logWidget->logMessage("Drone '" + name + "' created with autonomy " + QString::number(autonomy) + "km");
    }
}

void MainWindow::on_createVanButton_clicked()
{
    QString name = vehicleCreator->createVan();

    if (!name.isEmpty()) {
        mFleetManager.createVan(name.toStdString());
        refreshVehicleTable();
         logWidget->logMessage("Van '" + name + "' created");
    }
}

void MainWindow::on_createRobotButton_clicked()
{
    QString name = vehicleCreator->createRobot();

    if (!name.isEmpty()) {
        mFleetManager.createRobot(name.toStdString());
        refreshVehicleTable();
         logWidget->logMessage("Robot '" + name + "' created");
    }
}

void MainWindow::on_removeVehicleButton_clicked()
{
    int currentRow = ui->vehicleTableWidget->currentRow();
    if (currentRow >= 0 && currentRow < ui->vehicleTableWidget->rowCount()) {
        QTableWidgetItem* indexItem = ui->vehicleTableWidget->item(currentRow, 0);
        if (indexItem && indexItem->text() != "No vehicles in fleet") {
            bool ok;
            int vehicleIndex = indexItem->text().toInt(&ok);

            if (ok && vehicleIndex >= 0) {
                QString vehicleName = ui->vehicleTableWidget->item(currentRow, 1)->text();

                int packageCount = mFleetManager.getVehiclePackageCount(vehicleIndex);

                if (vehicleOperations->removeVehicle(vehicleIndex, vehicleName, packageCount)) {
                    mFleetManager.removeVehicle(vehicleIndex);
                    refreshVehicleTable();
                    logWidget->logMessage("Vehicle '" + vehicleName + "' removed from fleet");
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle to remove");
    }
}

void MainWindow::on_setStateButton_clicked()
{
    int vehicleRow = ui->vehicleTableWidget->currentRow();
    if (vehicleRow >= 0) {
        QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
        if (vehicleIndexItem && vehicleIndexItem->text() != "No vehicles in fleet") {
            bool ok;
            int vehicleIndex = vehicleIndexItem->text().toInt(&ok);
            if (ok && vehicleIndex >= 0) {
                QString currentState = ui->vehicleTableWidget->item(vehicleRow, 5)->text();
                QString vehicleName = ui->vehicleTableWidget->item(vehicleRow, 1)->text();

                // Get selected state from component
                QString selectedState = vehicleOperations->changeVehicleState(vehicleIndex,
                                                                              vehicleName,
                                                                              currentState);

                if (!selectedState.isEmpty()) {
                    // Convert string to VehicleState enum
                    VehicleState newState = VehicleState::IN_SERVICE;
                    if (selectedState == "Out of service")
                        newState = VehicleState::OUT_OF_SERVICE;
                    else if (selectedState == "Awaiting delivery")
                        newState = VehicleState::AWAITING_DELIVERY;

                    // Update fleet manager
                    mFleetManager.setVehicleState(vehicleIndex, newState);

                    // Refresh and log
                    refreshVehicleTable();
                    logWidget->logMessage(QString("Vehicle '%1' state changed from '%2' to '%3'")
                                   .arg(vehicleName, currentState, selectedState));
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle");
    }
}

void MainWindow::on_deliverButton_clicked()
{
    int vehicleRow = ui->vehicleTableWidget->currentRow();
    if (vehicleRow >= 0) {
        QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
        QTableWidgetItem* stateItem = ui->vehicleTableWidget->item(vehicleRow, 5);

        if (vehicleIndexItem && vehicleIndexItem->text() != "No vehicles in fleet") {
            // Check if vehicle is out of service
            if (stateItem && stateItem->text().contains("Out of service", Qt::CaseInsensitive)) {
                QMessageBox::warning(this, "Error", "Cannot deliver packages. Vehicle is OUT OF SERVICE.");
                return;
            }

            bool ok;
            int vehicleIndex = vehicleIndexItem->text().toInt(&ok);
            if (ok && vehicleIndex >= 0) {
                QString vehicleName = ui->vehicleTableWidget->item(vehicleRow, 1)->text();
                int packageCount = mFleetManager.getVehiclePackageCount(vehicleIndex);

                if (vehicleOperations->deliverPackages(vehicleIndex, vehicleName, packageCount)) {
                    mFleetManager.deliverPackages(vehicleIndex);
                    refreshVehicleTable();
                    logWidget->logMessage(QString("%1 package(s) delivered from vehicle '%2'")
                                   .arg(packageCount).arg(vehicleName));
                    QMessageBox::information(this, "Delivery Complete",
                                             QString("%1 package(s) successfully delivered from vehicle '%2'")
                                                 .arg(packageCount).arg(vehicleName));
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle");
    }
}
// =============================================
// Package Management
// =============================================

void MainWindow::on_createStandardPackageButton_clicked()
{
    handlePackageCreation("Standard",
                          [this]() { return packageCreator->createStandardPackage(); });
}

void MainWindow::on_createFragilePackageButton_clicked()
{
    handlePackageCreation("Fragile",
                          [this]() { return packageCreator->createFragilePackage(); });
}

void MainWindow::on_createRefrigeratedPackageButton_clicked()
{
    handlePackageCreation("Refrigerated",
                          [this]() { return packageCreator->createRefrigeratedPackage(); });
}

void MainWindow::on_createExpressPackageButton_clicked()
{
    handlePackageCreation("Express",
                          [this]() { return packageCreator->createExpressPackage(); });
}

// I wanted to apply the DRY (Don't Repeat Yourself) conecpt
// so this is a centralized handler for creating any package type
// Eliminates duplicate code across all package creation buttons
// Takes: package type name and a factory function that creates the package
// Returns: nothing (package is added to fleet manager and UI is updated)
void MainWindow::handlePackageCreation(const QString& packageType,
                                       std::function<std::unique_ptr<Package>()> createFunction)
{
    std::unique_ptr<Package> package = createFunction();
    if (package) {
        QString desc = QString::fromStdString(package->getDescription());
        double weight = package->getWeight();
        QString typeForLog = packageType.isEmpty()
                                 ? QString::fromStdString(package->getType())
                                 : packageType;

        mFleetManager.addPendingPackage(std::move(package));
        refreshPackageTable();
        logWidget->logMessage(
            QString("%1 package '%2' created (%3g)")
                .arg(typeForLog, desc, QString::number(weight))
            );
    }
}

// =============================================
// Assign Remove Packages
// =============================================

void MainWindow::on_assignPackageButton_clicked()
{
    const int vehicleRow = ui->vehicleTableWidget->currentRow();
    const int packageRow = ui->pendingPackagesTableWidget->currentRow();

    // --- Selection checks ---
    if (vehicleRow < 0 || packageRow < 0) {
        QString errorMsg = "Please select ";
        if (vehicleRow < 0 && packageRow < 0)
            errorMsg += "both a vehicle (from Vehicle Fleet table) and a package (from Pending Packages table)";
        else if (vehicleRow < 0)
            errorMsg += "a vehicle from the Vehicle Fleet table";
        else
            errorMsg += "a package from the Pending Packages table";

        QMessageBox::warning(this, "Selection Required", errorMsg);
        return;
    }

    // --- Index items ---
    QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
    QTableWidgetItem* packageIndexItem = ui->pendingPackagesTableWidget->item(packageRow, 0);

    if (!vehicleIndexItem || !packageIndexItem) {
        QMessageBox::warning(this, "Error", "Please select valid vehicle and package");
        return;
    }

    if (vehicleIndexItem->text() == "No vehicles in fleet" ||
        packageIndexItem->text() == "No pending packages") {
        QMessageBox::warning(this, "Error", "Please select valid vehicle and package");
        return;
    }

    // --- Parse indices ---
    bool vehicleOk = false, packageOk = false;
    const int vehicleIndex = vehicleIndexItem->text().toInt(&vehicleOk);
    const int packageIndex = packageIndexItem->text().toInt(&packageOk);

    if (!vehicleOk || !packageOk || vehicleIndex < 0 || packageIndex < 0) {
        QMessageBox::warning(this, "Error",
                             QString("Invalid indices:\nVehicle: %1\nPackage: %2")
                                 .arg(vehicleIndex).arg(packageIndex));
        return;
    }

    // --- Gather package info ---
    const QString vehicleName = ui->vehicleTableWidget->item(vehicleRow, 1)->text();
    const QString packageType = ui->pendingPackagesTableWidget->item(packageRow, 1)->text();
    const QString packageDesc = ui->pendingPackagesTableWidget->item(packageRow, 2)->text();

    // Weight parsing
    double packageWeight = 0.0;
    if (QTableWidgetItem* weightItem = ui->pendingPackagesTableWidget->item(packageRow, 3)) {
        QStringView weightView = QStringView(weightItem->text()).trimmed();
        if (weightView.endsWith('g')) weightView = weightView.chopped(1);
        packageWeight = weightView.toDouble();
    }

    // --- Vehicle state ---
    const double remainingCapacity = mFleetManager.getVehicleRemainingCapacity(vehicleIndex);
    const bool isOutOfService = ui->vehicleTableWidget->item(vehicleRow, 5)
                                && ui->vehicleTableWidget->item(vehicleRow, 5)->text()
                                       .contains("Out of service", Qt::CaseInsensitive);

    // --- Log attempt ---
    logWidget->logMessage(QString("Attempting to assign package %1 (%2g) to vehicle %3 (capacity : %4g)")
                              .arg(packageIndex).arg(packageWeight).arg(vehicleIndex).arg(remainingCapacity));

    // --- Validate with component ---
    auto result = packageOperations->assignPackage(vehicleIndex, packageIndex,
                                                   vehicleName, packageDesc, packageType,
                                                   packageWeight, remainingCapacity,
                                                   isOutOfService);

    if (!result.first) {
        QMessageBox::warning(this, "Assignment Failed", result.second);
        return;
    }

    // --- Perform assignment ---
    if (mFleetManager.addPackageToVehicle(vehicleIndex, packageIndex)) {
        refreshVehicleTable();
        refreshPackageTable();
        logWidget->logMessage(QString("✓ Package '%1' assigned to vehicle '%2'")
                                  .arg(packageDesc, vehicleName));
        QMessageBox::information(this, "Success",
                                 QString("Package '%1'\nSuccessfully assigned to vehicle '%2'")
                                     .arg(packageDesc, vehicleName));
    }
}


void MainWindow::on_removePackageButton_clicked()
{
    int vehicleRow = ui->vehicleTableWidget->currentRow();
    if (vehicleRow >= 0) {
        QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
        QTableWidgetItem* stateItem = ui->vehicleTableWidget->item(vehicleRow, 5);

        if (vehicleIndexItem && vehicleIndexItem->text() != "No vehicles in fleet") {
            // Check if vehicle is out of service
            if (stateItem && stateItem->text().contains("Out of service", Qt::CaseInsensitive)) {
                QMessageBox::warning(this, "Error", "Cannot remove packages. Vehicle is OUT OF SERVICE.");
                return;
            }

            bool ok;
            int vehicleIndex = vehicleIndexItem->text().toInt(&ok);
            if (ok && vehicleIndex >= 0) {
                QString vehicleName = ui->vehicleTableWidget->item(vehicleRow, 1)->text();

                // Get confirmation from component
                if (packageOperations->confirmRemovePackage(vehicleName)) {
                    if (mFleetManager.removeLastPackageToPending(vehicleIndex)) {
                        refreshVehicleTable();
                        refreshPackageTable();
                         logWidget->logMessage("Last package removed from vehicle '" + vehicleName + "'");
                        QMessageBox::information(this, "Success",
                                                 "Last package removed from vehicle and returned to pending list.");
                    } else {
                        QMessageBox::warning(this, "Warning",
                                             "No packages to remove from this vehicle or vehicle is out of service.");
                    }
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle");
    }
}

// =============================================
// Display
// =============================================

void MainWindow::on_refreshButton_clicked()
{
    logWidget->logMessage("Display refreshed");
}

void MainWindow::on_showVehicleDetailsButton_clicked()
{
    int vehicleRow = ui->vehicleTableWidget->currentRow();
    if (vehicleRow >= 0) {
        QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
        if (vehicleIndexItem && vehicleIndexItem->text() != "No vehicles in fleet") {
            bool ok;
            int vehicleIndex = vehicleIndexItem->text().toInt(&ok);
            if (ok && vehicleIndex >= 0) {
                const Vehicle* vehicle = mFleetManager.getVehicle(vehicleIndex);
                if (vehicle) {
                    vehicleInfoViewer->showDetails(vehicle);
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle");
    }
}

void MainWindow::on_showCompatibilityButton_clicked()
{
    int vehicleRow = ui->vehicleTableWidget->currentRow();
    if (vehicleRow >= 0) {
        QTableWidgetItem* vehicleIndexItem = ui->vehicleTableWidget->item(vehicleRow, 0);
        if (vehicleIndexItem && vehicleIndexItem->text() != "No vehicles in fleet") {
            bool ok;
            int vehicleIndex = vehicleIndexItem->text().toInt(&ok);
            if (ok && vehicleIndex >= 0) {
                const Vehicle* vehicle = mFleetManager.getVehicle(vehicleIndex);
                if (vehicle) {
                    vehicleInfoViewer->showCompatibility(vehicle, mFleetManager);
                }
            }
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a vehicle");
    }
}
