#include "FleetManager.h"
#include <sstream>
#include <stdexcept>

// for my backen class i tried to make it optimal using c++17 good practice
// so methods that display or return information are const

// Create different vehicle types using smart pointers
void FleetManager::createDrone(const std::string& name, double autonomy) {
    // Validate autonomy before creating drone
    if (autonomy <= 0) {
        throw std::invalid_argument("Drone autonomy must be positive");
    }

    // Validate name before creating any vehicle
    if (name.empty()) {
        throw std::invalid_argument("Vehicle name cannot be empty");
    }

    mVehicles.push_back(std::make_unique<Drone>(name, autonomy));  // make_unique for exception safety
}

void FleetManager::createVan(const std::string& name) {
    // Validate name before creating any vehicle
    if (name.empty()) {
        throw std::invalid_argument("Vehicle name cannot be empty");
    }

    mVehicles.push_back(std::make_unique<Van>(name));
}

void FleetManager::createRobot(const std::string& name) {
    // Validate name before creating any vehicle
    if (name.empty()) {
        throw std::invalid_argument("Vehicle name cannot be empty");
    }

    mVehicles.push_back(std::make_unique<Robot>(name));
}

void FleetManager::removeVehicle(int index) {
    // Bounds checking before erase
    if (index < 0 || index >= static_cast<int>(mVehicles.size())) {
        throw std::out_of_range("Vehicle index out of range");
    }
    mVehicles.erase(mVehicles.begin() + index);
}

// Add package to vehicle with compatibility check
bool FleetManager::addPackageToVehicle(int vehicleIndex, int packageIndex) {
    // First check bounds for both indices
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(mVehicles.size())) {
        return false;
    }

    if (packageIndex < 0 || packageIndex >= static_cast<int>(mPendingPackages.size())) {
        return false;
    }

    // Check if package exists (nullptr check)
    if (!mPendingPackages[packageIndex]) {
        return false;
    }

    // Check if vehicle is out of service
    if (mVehicles[vehicleIndex]->getState() == VehicleState::OUT_OF_SERVICE) {
        return false;  // Can't assign to out-of-service vehicle
    }

    // Check compatibility BEFORE moving package
    if (!isPackageCompatibleWithVehicle(vehicleIndex, packageIndex)) {
        return false;
    }

    // Now safely move the package
    if (mVehicles[vehicleIndex]->addPackage(std::move(mPendingPackages[packageIndex]))) {
        mPendingPackages.erase(mPendingPackages.begin() + packageIndex);
        return true;
    }

    return false;
}

// Move last package from vehicle back to pending list
bool FleetManager::removeLastPackageToPending(int vehicleIndex) {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(mVehicles.size())) {
        return false;  // Bounds check
    }

    // Check if vehicle is out of service
    if (mVehicles[vehicleIndex]->getState() == VehicleState::OUT_OF_SERVICE) {
        return false;  // Can't remove from out-of-service vehicle
    }

    auto removedPackage = mVehicles[vehicleIndex]->removeLastPackage();
    if (removedPackage) {
        mPendingPackages.push_back(std::move(removedPackage));  // Move semantics
        return true;
    }

    return false;
}

void FleetManager::deliverPackages(int vehicleIndex) {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(mVehicles.size())) {
        return;  // Bounds check
    }

    auto& vehicle = mVehicles[vehicleIndex];

    // Check if vehicle is out of service
    if (vehicle->getState() == VehicleState::OUT_OF_SERVICE) {
        return;  // Can't deliver packages from out-of-service vehicle
    }

    vehicle->deliverAllPackages();  // Package destruction handled by Vehicle
}

void FleetManager::setVehicleState(int vehicleIndex, VehicleState state) {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(mVehicles.size())) {
        throw std::out_of_range("Vehicle index out of range");
    }

    mVehicles[vehicleIndex]->setState(state);
}

// Display all vehicles summary - NOW RETURNS STRING
std::string FleetManager::displayAllVehicles() const {
    std::ostringstream oss;
    if (mVehicles.empty()) {
        oss << "No vehicles in the fleet.\n";
        return oss.str();
    }

    oss << "=== Fleet (" << mVehicles.size() << " vehicles) ===\n";
    for (size_t i = 0; i < mVehicles.size(); ++i) {
        oss << i << ". ";
        oss << mVehicles[i]->displayInfo();  // Polymorphic call
        oss << "\n";
    }
    return oss.str();
}

std::string FleetManager::displayVehicleDetails(int index) const {
    std::ostringstream oss;
    if (index >= 0 && index < static_cast<int>(mVehicles.size())) {
        oss << "\n=== Vehicle details ===\n";
        oss << mVehicles[index]->displayDetails();  // Includes package details
        oss << "\n";
    } else {
        oss << "Invalid index!\n";
    }
    return oss.str();
}

size_t FleetManager::getVehicleCount() const {
    return mVehicles.size();
}

// Add package to pending list
void FleetManager::addPendingPackage(std::unique_ptr<Package> package) {
    // Package null check
    if (!package) {
        throw std::invalid_argument("Cannot add null package to pending list");
    }

    mPendingPackages.push_back(std::move(package));
}

// Display all pending packages
std::string FleetManager::displayPendingPackages() const {
    std::ostringstream oss;
    if (mPendingPackages.empty()) {
        oss << "No pending packages.\n";
    } else {
        oss << "Number of pending packages: " << mPendingPackages.size() << "\n\n";
        for (size_t i = 0; i < mPendingPackages.size(); ++i) {
            oss << "[" << i << "] ";
            oss << mPendingPackages[i]->displayInfo();  // Polymorphic call
            oss << "\n";
        }
    }
    return oss.str();
}

size_t FleetManager::getPendingPackageCount() const {
    return mPendingPackages.size();
}

// Check if package fits in vehicle's remaining capacity
bool FleetManager::isPackageCompatibleWithVehicle(int vehicleIndex, int packageIndex) const {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(mVehicles.size()) ||
        packageIndex < 0 || packageIndex >= static_cast<int>(mPendingPackages.size())) {
        return false;
    }

    const auto& vehicle = mVehicles[vehicleIndex];      // const reference
    const auto& package = mPendingPackages[packageIndex];

    // Additional nullptr check
    if (!package) {
        return false;
    }

    return package->getWeight() <= (vehicle->getMaxCapacity() - vehicle->getCurrentLoad());
}

// Return const reference to vehicles vector
const std::vector<std::unique_ptr<Vehicle>>& FleetManager::getAllVehicles() const {
    return mVehicles;
}

// Return const reference to packages vector
const std::vector<std::unique_ptr<Package>>& FleetManager::getAllPendingPackages() const {
    return mPendingPackages;
}

// Get specific vehicle by index (safe)
const Vehicle* FleetManager::getVehicle(int index) const {
    if (index >= 0 && index < static_cast<int>(mVehicles.size())) {
        return mVehicles[index].get();  // Get raw pointer from unique_ptr
    }
    return nullptr;
}

// Get specific package by index (safe)
const Package* FleetManager::getPendingPackage(int index) const {
    if (index >= 0 && index < static_cast<int>(mPendingPackages.size())) {
        return mPendingPackages[index].get();
    }
    return nullptr;
}

// Helper methods for UI
double FleetManager::getVehicleRemainingCapacity(int vehicleIndex) const {
    if (vehicleIndex >= 0 && vehicleIndex < static_cast<int>(mVehicles.size())) {
        const auto& vehicle = mVehicles[vehicleIndex];
        return vehicle->getMaxCapacity() - vehicle->getCurrentLoad();
    }
    return 0.0;
}

int FleetManager::getVehiclePackageCount(int vehicleIndex) const {
    if (vehicleIndex >= 0 && vehicleIndex < static_cast<int>(mVehicles.size())) {
        return static_cast<int>(mVehicles[vehicleIndex]->getPackageCount());
    }
    return 0;
}

// Display compatibility info for UI - NOW RETURNS STRING
std::string FleetManager::displayCompatibilityInfo(int vehicleIndex) const {
    std::ostringstream oss;
    if (vehicleIndex >= 0 && vehicleIndex < static_cast<int>(mVehicles.size())) {
        const auto& vehicle = mVehicles[vehicleIndex];
        double remainingCapacity = vehicle->getMaxCapacity() - vehicle->getCurrentLoad();

        // Check vehicle state
        if (vehicle->getState() == VehicleState::OUT_OF_SERVICE) {
            oss << "\nVehicle is OUT OF SERVICE - cannot accept packages\n";
            return oss.str();
        }

        oss << "\nAvailable packages (remaining capacity: " << remainingCapacity << "g):\n";

        for (size_t i = 0; i < mPendingPackages.size(); ++i) {
            const auto& package = mPendingPackages[i];

            // Check if package exists
            if (!package) {
                continue;
            }

            bool compatible = package->getWeight() <= remainingCapacity;

            oss << "[" << i << "] ";
            oss << package->displayInfo();  // Polymorphic display
            oss << (compatible ? " (compatible)\n" : " (too heavy)\n");
        }
    }
    return oss.str();
}

// Calculate total maximum capacity of all vehicles
double FleetManager::getTotalFleetCapacity() const {
    double total = 0.0;
    for (const auto& vehicle : mVehicles) {
        total += vehicle->getMaxCapacity();
    }
    return total;
}

// Calculate total used capacity (current load) of all vehicles
double FleetManager::getUsedCapacity() const {
    double used = 0.0;
    for (const auto& vehicle : mVehicles) {
        used += vehicle->getCurrentLoad();
    }
    return used;
}
