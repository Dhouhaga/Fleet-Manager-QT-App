#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>

// Fragility levels for fragile packages
enum class FragilityLevel {
    LOW,
    MEDIUM,
    HIGH
};

// Abstract base class for all package types
class Package {
protected:
    std::string mDescription;  // Text description
    double mWeight;            // Weight in grams

public:
    Package(const std::string& desc, double w);
    virtual ~Package() = default;

    // Delete copy operations to prevent copying
    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

    // Allow move operations for efficient transfers avoiding copies
    Package(Package&&) = default;
    Package& operator=(Package&&) = default;

    // virtual methods for polymorphic display
    virtual std::string displayInfo() const = 0;
    virtual std::string getType() const = 0;

    // Getters const because they won't change the object
    double getWeight() const;
    const std::string& getDescription() const;  // Return by const reference --> no copies
};

// Standard package without special constraints
class StandardPackage : public Package {
public:
    StandardPackage(const std::string& desc, double w);
    std::string displayInfo() const override;
    std::string getType() const override;
};

// Fragile package with fragility level
class FragilePackage : public Package {
    FragilityLevel mFragility;  // Specific property for fragile packages

public:
    FragilePackage(const std::string& desc, double w, FragilityLevel f);
    std::string displayInfo() const override;
    std::string getType() const override;
    FragilityLevel getFragility() const;  // Getter for fragility level
    static std::string fragilityToString(FragilityLevel level);
};

// Refrigerated package with target temperature
class RefrigeratedPackage : public Package {
    double mTargetTemperature;  // Temperature in Celsius

public:
    RefrigeratedPackage(const std::string& desc, double w, double temp);
    std::string displayInfo() const override;
    std::string getType() const override;
    double getTemperature() const;  // Getter for temperature
};

// Express package (high priority)
class ExpressPackage : public Package {
public:
    ExpressPackage(const std::string& desc, double w);
    std::string displayInfo() const override;
    std::string getType() const override;
};

#endif

