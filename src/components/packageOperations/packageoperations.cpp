#include "PackageOperations.h"
#include <QMessageBox>

PackageOperations::PackageOperations(QWidget *parent) : QWidget(parent) {}

std::pair<bool, QString> PackageOperations::assignPackage(int vehicleIndex, int packageIndex,
                                                          const QString& vehicleName,
                                                          const QString& packageDesc,
                                                          const QString& packageType,
                                                          double packageWeight,
                                                          double remainingCapacity,
                                                          bool isVehicleOutOfService)
{
    // Check if vehicle is out of service
    if (isVehicleOutOfService) {
        return std::make_pair(false, createOutOfServiceError(vehicleName));
    }

    // Check capacity
    if (packageWeight > remainingCapacity) {
        return std::make_pair(false, createWeightError(packageDesc, packageType, vehicleName,
                                                       packageWeight, remainingCapacity));
    }

    // All checks passed
    return std::make_pair(true, QString());
}

bool PackageOperations::confirmRemovePackage(const QString& vehicleName)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Removal",
                                  QString("Remove the last package from vehicle '%1'?").arg(vehicleName),
                                  QMessageBox::Yes | QMessageBox::No);

    return (reply == QMessageBox::Yes);
}

QString PackageOperations::createWeightError(const QString& packageDesc, const QString& packageType,
                                             const QString& vehicleName, double packageWeight,
                                             double remainingCapacity) const
{
    return QString("Package too heavy!\n\n"
                   "Package: %1 (%2)\n"
                   "Vehicle: %3\n"
                   "Package weight: %4g\n"
                   "Vehicle remaining capacity: %5g\n\n"
                   "The package exceeds the vehicle's available capacity by %6g.")
        .arg(packageDesc, packageType, vehicleName,
             QString::number(packageWeight),
             QString::number(remainingCapacity))
        .arg(packageWeight - remainingCapacity);
}

QString PackageOperations::createOutOfServiceError(const QString& vehicleName) const
{
    return QString("Vehicle is OUT OF SERVICE!\n\n"
                   "Vehicle: %1\n"
                   "Status: Out of service\n\n"
                   "Out of service vehicles cannot accept packages.")
        .arg(vehicleName);
}

