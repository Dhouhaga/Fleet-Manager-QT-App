#include "VehicleInfoViewer.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QFont>
#include <QDialog>
#include <QDialogButtonBox>

VehicleInfoViewer::VehicleInfoViewer(QWidget *parent) : QWidget(parent){}

void VehicleInfoViewer::showDetails(const Vehicle* vehicle)
{
    if (!vehicle) return;

    QString details = formatVehicleDetails(vehicle);

    // Create and show dialog with details
    QDialog dialog(this->parentWidget());
    dialog.setWindowTitle("Vehicle Details");
    dialog.setMinimumWidth(600);
    dialog.setMinimumHeight(400);

    QVBoxLayout layout(&dialog);
    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setText(details);
    textEdit->setReadOnly(true);
    textEdit->setFont(QFont("Courier New", 10));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    layout.addWidget(textEdit);
    layout.addWidget(buttonBox);

    dialog.exec();
}

void VehicleInfoViewer::showCompatibility(const Vehicle* vehicle, const FleetManager& fleetManager)
{
    if (!vehicle) return;

    QString compatInfo = formatCompatibilityInfo(vehicle, fleetManager);

    // Create and show dialog with compatibility info
    QDialog dialog(this->parentWidget());
    dialog.setWindowTitle("Compatibility Information");
    dialog.setMinimumWidth(700);
    dialog.setMinimumHeight(400);

    QVBoxLayout layout(&dialog);
    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setText(compatInfo);
    textEdit->setReadOnly(true);
    textEdit->setFont(QFont("Courier New", 10));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    layout.addWidget(textEdit);
    layout.addWidget(buttonBox);

    dialog.exec();
}

QString VehicleInfoViewer::formatVehicleDetails(const Vehicle* vehicle) const
{
    if (!vehicle) return QString();

    QString text;

    // Basic info
    text += QString("=== Vehicle Details ===\n\n");
    text += QString("ID: %1\n").arg(vehicle->getId());
    text += QString("Name: %1\n").arg(QString::fromStdString(vehicle->getName()));
    text += QString("Type: %1\n").arg(QString::fromStdString(vehicle->getType()));
    text += QString("Max Capacity: %1g\n").arg(vehicle->getMaxCapacity(), 0, 'f', 1);
    text += QString("Current Load: %1g\n").arg(vehicle->getCurrentLoad(), 0, 'f', 1);
    text += QString("Available: %1g\n").arg(vehicle->getMaxCapacity() - vehicle->getCurrentLoad(), 0, 'f', 1);

    // State
    QString state;
    switch (vehicle->getState()) {
    case VehicleState::IN_SERVICE: state = "In service"; break;
    case VehicleState::OUT_OF_SERVICE: state = "Out of service"; break;
    case VehicleState::AWAITING_DELIVERY: state = "Awaiting delivery"; break;
    }
    text += QString("State: %1\n").arg(state);

    // Specific info (polymorphic)
    text += QString("Specific Details: %1\n\n").arg(QString::fromStdString(vehicle->getSpecificInfo()));

    // Packages
    text += QString("\nPackages (%1):\n").arg(vehicle->getPackageCount());
    const auto& packages = vehicle->getPackages();

    for (size_t i = 0; i < packages.size(); ++i) {
        const Package* package = packages[i].get();
        text += QString("  [%1] ").arg(i + 1);
        text += QString::fromStdString(package->getDescription());
        text += QString(" (%1g, %2)\n")
                    .arg(package->getWeight(), 0, 'f', 1)
                    .arg(QString::fromStdString(package->getType()));
    }

    return text;
}

QString VehicleInfoViewer::formatCompatibilityInfo(const Vehicle* vehicle, const FleetManager& fleetManager) const
{
    if (!vehicle) return QString();

    QString text;

    if (vehicle->getState() == VehicleState::OUT_OF_SERVICE) {
        text += "Vehicle is OUT OF SERVICE - cannot accept packages\n";
        return text;
    }

    double remainingCapacity = vehicle->getMaxCapacity() - vehicle->getCurrentLoad();
    text += QString("Available packages (remaining capacity: %1g):\n\n").arg(remainingCapacity, 0, 'f', 1);

    const auto& packages = fleetManager.getAllPendingPackages();
    for (size_t i = 0; i < packages.size(); ++i) {
        const Package* package = packages[i].get();
        bool compatible = package->getWeight() <= remainingCapacity;

        text += QString("[%1] %2 - %3g")
                    .arg(i)
                    .arg(QString::fromStdString(package->getDescription()))
                    .arg(package->getWeight(), 0, 'f', 1);

        text += compatible ? " (compatible)\n" : " (too heavy)\n";
    }

    return text;
}
