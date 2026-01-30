#ifndef VEHICLEOPERATIONS_H
#define VEHICLEOPERATIONS_H

#include <QWidget>
#include <QString>
#include "../../core/FleetManager.h"

class VehicleOperations : public QWidget
{
    Q_OBJECT

public:
    explicit VehicleOperations(QWidget *parent = nullptr);

    bool removeVehicle(int vehicleIndex, const QString& vehicleName, int packageCount);
    QString changeVehicleState(int vehicleIndex, const QString& vehicleName,
                               const QString& currentState);  // Returns selected state or empty if cancelled
    bool deliverPackages(int vehicleIndex, const QString& vehicleName,
                         int packageCount);

private:
    // Helper to check if vehicle is out of service
    bool isVehicleOutOfService(const QString& stateText) const;
};

#endif
