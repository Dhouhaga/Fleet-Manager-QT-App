#ifndef VEHICLE_H
#define VEHICLE_H

#include "Package.h"
#include <vector>
#include <memory>
#include <string>

// Enum for vehicle states
enum class VehicleState {
    IN_SERVICE,
    OUT_OF_SERVICE,
    AWAITING_DELIVERY
};

class Vehicle {
protected:
    static int sNextId;              // Static counter for automatic unique IDs
    int mId;                         // Unique identifier for each vehicle
    std::string mName;               // Vehicle name (unique per requirements)
    double mMaxCapacity;             // Max capacity in grams
    double mCurrentLoad;             // Current load in grams
    VehicleState mState;             // Current state of the vehicle
    std::vector<std::unique_ptr<Package>> mPackages;  // Collection of packages

public:
    Vehicle(const std::string& n, double capacity);
    virtual ~Vehicle() = default;

    // Delete copy operations to prevent copying
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;

    // Allow move operations for efficient transfers
    Vehicle(Vehicle&&) = default;
    Vehicle& operator=(Vehicle&&) = default;

    // Pure virtual methods for polymorphic behavior
    virtual std::string getType() const = 0;
    virtual std::string getSpecificInfo() const = 0;  

    // Package management methods
    bool addPackage(std::unique_ptr<Package> package);
    std::unique_ptr<Package> removeLastPackage();
    void deliverAllPackages();      // Destroys all packages and resets state
    void setState(VehicleState newState);
    std::string displayInfo() const;       
    std::string displayDetails() const;    

    // Getters const because they won't change the object
    int getId() const;
    const std::string& getName() const;  // Return by const reference to avoid copy
    double getMaxCapacity() const;
    double getCurrentLoad() const;
    VehicleState getState() const;
    size_t getPackageCount() const;
    const std::vector<std::unique_ptr<Package>>& getPackages() const;
};

// Drone class with specific autonomy property
class Drone : public Vehicle {
    double mAutonomy;              

public:
    Drone(const std::string& n, double aut);
    std::string getType() const override;
    std::string getSpecificInfo() const override;
};

// Van class - electric ground vehicle
class Van : public Vehicle {
public:
    Van(const std::string& n);
    std::string getType() const override;
    std::string getSpecificInfo() const override;
};

// Robot class - urban autonomous robot
class Robot : public Vehicle {
public:
    Robot(const std::string& n);
    std::string getType() const override;
    std::string getSpecificInfo() const override;
};

#endif

