#ifndef PACKAGEOPERATIONS_H
#define PACKAGEOPERATIONS_H

#include <QWidget>
#include <QString>
#include <utility>

class PackageOperations : public QWidget
{
    Q_OBJECT

public:
    explicit PackageOperations(QWidget *parent = nullptr);

    // Package assignment - returns (success, errorMessage)
    std::pair<bool, QString> assignPackage(int vehicleIndex, int packageIndex,
                                           const QString& vehicleName,
                                           const QString& packageDesc,
                                           const QString& packageType,
                                           double packageWeight,
                                           double remainingCapacity,
                                           bool isVehicleOutOfService);

    // Package removal confirmation
    bool confirmRemovePackage(const QString& vehicleName);

private:
    // Helper methods for error messages
    QString createWeightError(const QString& packageDesc, const QString& packageType,
                              const QString& vehicleName, double packageWeight,
                              double remainingCapacity) const;
    QString createOutOfServiceError(const QString& vehicleName) const;
};

#endif
