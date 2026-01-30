#include "Package.h"
#include <sstream>
#include <stdexcept> 

// Base constructor
Package::Package(const std::string& desc, double w)
    : mDescription(desc), mWeight(w) {
    if (mWeight <= 0) {
        throw std::invalid_argument("Package weight must be positive");
    }
    if (mDescription.empty()) {
        throw std::invalid_argument("Package description cannot be empty");
    }
}

// Getter implementations
double Package::getWeight() const { return mWeight; }

const std::string& Package::getDescription() const { return mDescription; }

// StandardPackage implementation
StandardPackage::StandardPackage(const std::string& desc, double w)
    : Package(desc, w) {
}

std::string StandardPackage::displayInfo() const {
    std::ostringstream oss;
    oss << "[STANDARD] " << mDescription << " - Weight: " << mWeight << "g";
    return oss.str();
}

std::string StandardPackage::getType() const {
    return "Standard";
}

// FragilePackage implementation
FragilePackage::FragilePackage(const std::string& desc, double w, FragilityLevel f)
    : Package(desc, w), mFragility(f) {
}

std::string FragilePackage::displayInfo() const {
    std::ostringstream oss;
    oss << "[FRAGILE] " << mDescription << " - Weight: " << mWeight << "g - Fragility: ";
    switch (mFragility) {
    case FragilityLevel::LOW: oss << "Low"; break;
    case FragilityLevel::MEDIUM: oss << "Medium"; break;
    case FragilityLevel::HIGH: oss << "High"; break;
    }
    return oss.str();
}

std::string FragilePackage::getType() const {
    return "Fragile";
}

FragilityLevel FragilePackage::getFragility() const {
    return mFragility;
}

std::string FragilePackage::fragilityToString(FragilityLevel level) {
    switch (level) {
    case FragilityLevel::LOW: return "Low";
    case FragilityLevel::MEDIUM: return "Medium";
    case FragilityLevel::HIGH: return "High";
    default: return "Unknown";
    }
}

// RefrigeratedPackage implementation
RefrigeratedPackage::RefrigeratedPackage(const std::string& desc, double w, double temp)
    : Package(desc, w), mTargetTemperature(temp) {
    if (temp < -50.0 || temp > 10.0) {
        throw std::invalid_argument("Refrigerated package temperature must be between -50°C and +10°C");
    }
}

std::string RefrigeratedPackage::displayInfo() const {
    std::ostringstream oss;
    oss << "[REFRIGERATED] " << mDescription << " - Weight: " << mWeight
        << "g - Temperature: " << mTargetTemperature << "°C";
    return oss.str();
}

std::string RefrigeratedPackage::getType() const {
    return "Refrigerated";
}

double RefrigeratedPackage::getTemperature() const {
    return mTargetTemperature;
}

// ExpressPackage implementation
ExpressPackage::ExpressPackage(const std::string& desc, double w)
    : Package(desc, w) {
}

std::string ExpressPackage::displayInfo() const {
    std::ostringstream oss;
    oss << "[EXPRESS] " << mDescription << " - Weight: " << mWeight << "g - URGENT";
    return oss.str();
}

std::string ExpressPackage::getType() const {
    return "Express";
}
