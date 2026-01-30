#ifndef VEHICLEINFOVIEWER_H
#define VEHICLEINFOVIEWER_H

#include <QWidget>
#include "../../core/FleetManager.h"

class VehicleInfoViewer : public QWidget
{
    Q_OBJECT

public:
    explicit VehicleInfoViewer(QWidget *parent = nullptr);
    // No destructor needed

    void showDetails(const Vehicle* vehicle);
    void showCompatibility(const Vehicle* vehicle, const FleetManager& fleetManager);

private:
    QString formatVehicleDetails(const Vehicle* vehicle) const;
    QString formatCompatibilityInfo(const Vehicle* vehicle, const FleetManager& fleetManager) const;
};

#endif 

