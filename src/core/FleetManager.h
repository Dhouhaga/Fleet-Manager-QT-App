#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

#include "Vehicle.h"
#include <vector>
#include <memory>
#include <optional>

// Main class managing the fleet and pending packages
class FleetManager {
    std::vector<std::unique_ptr<Vehicle>> mVehicles;      // Collection of vehicles
    std::vector<std::unique_ptr<Package>> mPendingPackages; // Packages waiting for assignment

public:
    // Vehicle management methods
    void createDrone(const std::string& name, double autonomy);
    void createVan(const std::string& name);
    void createRobot(const std::string& name);

    void removeVehicle(int index);
    bool addPackageToVehicle(int vehicleIndex, int packageIndex);
    bool removeLastPackageToPending(int vehicleIndex);
    void deliverPackages(int vehicleIndex);
    void setVehicleState(int vehicleIndex, VehicleState state);

    // Display methods
    std::string displayAllVehicles() const;
    std::string displayVehicleDetails(int index) const;

    const std::vector<std::unique_ptr<Vehicle>>& getAllVehicles() const;
    const std::vector<std::unique_ptr<Package>>& getAllPendingPackages() const;
    const Vehicle* getVehicle(int index) const;
    const Package* getPendingPackage(int index) const;

    size_t getVehicleCount() const;

    // Package management methods
    void addPendingPackage(std::unique_ptr<Package> package);
    std::string displayPendingPackages() const;
    size_t getPendingPackageCount() const;
    bool isPackageCompatibleWithVehicle(int vehicleIndex, int packageIndex) const;

    // Helper methods for main UI
    double getVehicleRemainingCapacity(int vehicleIndex) const;
    int getVehiclePackageCount(int vehicleIndex) const;
    std::string displayCompatibilityInfo(int vehicleIndex) const;
    double getTotalFleetCapacity() const;
    double getUsedCapacity() const;

};

#endif
