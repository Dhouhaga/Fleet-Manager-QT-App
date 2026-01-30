#include "Vehicle.h"
#include <sstream>
#include <stdexcept>

// Initialize static member for unique ID generation
int Vehicle::sNextId {1};

// Constructor with member initialization list for efficiency
Vehicle::Vehicle(const std::string& n, double capacity)
    : mId(sNextId++),               // Auto-increment unique ID
    mName(n),                     // Store name
    mMaxCapacity(capacity),       // Set max capacity
    mCurrentLoad(0),              // Start with no load
    mState(VehicleState::IN_SERVICE) {  // Default state

    // Validate capacity
    if (mMaxCapacity <= 0) {
        throw std::invalid_argument("Vehicle capacity must be positive");
    }

    // Validate name
    if (mName.empty()) {
        throw std::invalid_argument("Vehicle name cannot be empty");
    }
}

// Add package if capacity allows
bool Vehicle::addPackage(std::unique_ptr<Package> package) {
    // Check if vehicle can accept packages in its current state
    if (mState == VehicleState::OUT_OF_SERVICE) {
        return false;  // Cannot add packages to out-of-service vehicles
    }

    if (mCurrentLoad + package->getWeight() <= mMaxCapacity) {
        mCurrentLoad += package->getWeight();
        mPackages.push_back(std::move(package));  // Move for efficiency
        mState = VehicleState::AWAITING_DELIVERY;
        return true;
    }
    return false;
}

// Remove last package (LIFO)
std::unique_ptr<Package> Vehicle::removeLastPackage() {
    if (mState == VehicleState::OUT_OF_SERVICE) {
        return nullptr;  // Can't remove from out-of-service vehicle
    }

    if (mPackages.empty()) {
        // Only set to IN_SERVICE if we're not already OUT_OF_SERVICE
        if (mState != VehicleState::OUT_OF_SERVICE) {
            mState = VehicleState::IN_SERVICE;
        }
        return nullptr;  // No packages to remove
    }

    mCurrentLoad -= mPackages.back()->getWeight();
    auto lastPackage = std::move(mPackages.back());  // Move semantics
    mPackages.pop_back();
    if (mPackages.empty()) {
        // Only set to IN_SERVICE if we're not already OUT_OF_SERVICE
        if (mState != VehicleState::OUT_OF_SERVICE) {
            mState = VehicleState::IN_SERVICE;
        }
    }
    return lastPackage;
}

// Deliver all packages (destroy them)
void Vehicle::deliverAllPackages() {
    // Check if vehicle can deliver in its current state
    if (mState == VehicleState::OUT_OF_SERVICE) {
        return;  // Cannot deliver packages from out-of-service vehicles
    }

    mPackages.clear();           // vector destructor handles package destruction
    mCurrentLoad = 0;
    mState = VehicleState::IN_SERVICE;
}

void Vehicle::setState(VehicleState newState) {
    mState = newState;
}

// Display summary information
std::string Vehicle::displayInfo() const {
    std::ostringstream oss;
    oss << "[" << mId << "] " << mName << " (" << getType() << ") - "
        << mCurrentLoad << "/" << mMaxCapacity << "g - ";

    switch (mState) {
    case VehicleState::IN_SERVICE:
        oss << "In service";
        break;
    case VehicleState::OUT_OF_SERVICE:
        oss << "Out of service";
        break;
    case VehicleState::AWAITING_DELIVERY:
        oss << "Awaiting delivery";
        break;
    }
    return oss.str();
}

// Display full details with polymorphic package display
std::string Vehicle::displayDetails() const {
    std::ostringstream oss;
    oss << displayInfo();
    oss << "\nSpecific details: ";
    oss << getSpecificInfo();  // Polymorphic call
    oss << "\nPackages (" << mPackages.size() << "):\n";

    for (const auto& pkg : mPackages) {
        oss << "  - ";
        oss << pkg->displayInfo();  // Polymorphic call for package display
        oss << "\n";
    }
    return oss.str();
}

// Getters implementation
int Vehicle::getId() const { return mId; }

const std::string& Vehicle::getName() const { return mName; }  // Return reference

double Vehicle::getMaxCapacity() const { return mMaxCapacity; }

double Vehicle::getCurrentLoad() const { return mCurrentLoad; }

VehicleState Vehicle::getState() const { return mState; }

size_t Vehicle::getPackageCount() const { return mPackages.size(); }

const std::vector<std::unique_ptr<Package>>& Vehicle::getPackages() const {
    return mPackages;
}

// Drone implementation with 5000g (5kg)
Drone::Drone(const std::string& n, double aut)
    : Vehicle(n, 5000), mAutonomy(aut) {

    // Validate autonomy
    if (mAutonomy <= 0) {
        throw std::invalid_argument("Drone autonomy must be positive");
    }
}

std::string Drone::getType() const {
    return "Drone";
}

std::string Drone::getSpecificInfo() const {
    std::ostringstream oss;
    oss << "Autonomy: " << mAutonomy << "km";
    return oss.str();
}

// Van implementation with 500000g (500kg)
Van::Van(const std::string& n) : Vehicle(n, 500000) {
}

std::string Van::getType() const {
    return "Van";
}

std::string Van::getSpecificInfo() const {
    return "Electric ground vehicle";
}

// Robot implementation with 20000g (20kg)
Robot::Robot(const std::string& n) : Vehicle(n, 20000) {
}

std::string Robot::getType() const {
    return "Robot";
}

std::string Robot::getSpecificInfo() const {
    return "Autonomous urban robot";
}

