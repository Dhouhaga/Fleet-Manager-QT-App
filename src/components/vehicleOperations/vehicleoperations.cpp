#include "VehicleOperations.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
VehicleOperations::VehicleOperations(QWidget *parent) : QWidget(parent) {}

bool VehicleOperations::removeVehicle(int vehicleIndex, const QString& vehicleName,
                                      int packageCount)
{
    if (packageCount > 0) {
        QMessageBox::warning(this, "Cannot Remove Vehicle",
                             QString("Vehicle '%1' cannot be removed because it has %2 package(s).\n"
                                     "Remove all packages first before deleting the vehicle.")
                                 .arg(vehicleName).arg(packageCount));
        return false;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Removal",
                                  QString("Are you sure you want to remove vehicle '%1'?").arg(vehicleName),
                                  QMessageBox::Yes | QMessageBox::No);

    return (reply == QMessageBox::Yes);
}

QString VehicleOperations::changeVehicleState(int vehicleIndex, const QString& vehicleName,
                                              const QString& currentState)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Set Vehicle State");

    dialog.setStyleSheet(R"(
        QDialog {
            background-color: #ffffff;
            border: 1px solid #bdc3c7;
            border-radius: 6px;
        }
        QLabel {
            color: #2c3e50;
            font-size: 10pt;
        }
        QComboBox {
            border: 1px solid #bdc3c7;
            border-radius: 3px;
            padding: 2px;
            font-size: 10pt;
            min-height: 17px;
            background-color: #ffffff;
        }
        QComboBox:focus {
            border: 2px solid #3498db;
        }
        QDialogButtonBox {
            background-color: transparent;
        }
        QPushButton {
            border-radius: 5px;
            padding: 8px 16px;
            min-height: 20px;
            font-size: 10pt;
            background-color: #b1b1b1;
            color: #2c3e50;
        }
        QPushButton:hover {
            background-color: #999999;
        }
        QPushButton:pressed {
            background-color: #777777;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QWidget *formContainer = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formContainer);
    formLayout->setSpacing(10);

    // Vehicle name display (read-only)
    QLabel *nameLabel = new QLabel("Vehicle:");
    nameLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

    QLabel *nameValue = new QLabel(vehicleName);
    nameValue->setStyleSheet("color: #34495e; font-size: 10pt;");
    nameValue->setMinimumHeight(30);
    formLayout->addRow(nameLabel, nameValue);

    // Current state display
    QLabel *currentStateLabel = new QLabel("Current State:");
    currentStateLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

    QLabel *currentStateValue = new QLabel(currentState);
    currentStateValue->setStyleSheet("color: #34495e; font-size: 10pt;");
    currentStateValue->setMinimumHeight(30);
    formLayout->addRow(currentStateLabel, currentStateValue);

    // State selection
    QLabel *newStateLabel = new QLabel("New State:");
    newStateLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

    QComboBox *stateComboBox = new QComboBox();
    stateComboBox->addItems({"In service", "Out of service", "Awaiting delivery"});
    stateComboBox->setMinimumHeight(30);

    // Set current selection
    for (int i = 0; i < stateComboBox->count(); ++i) {
        if (currentState.contains(stateComboBox->itemText(i), Qt::CaseInsensitive)) {
            stateComboBox->setCurrentIndex(i);
            break;
        }
    }

    formLayout->addRow(newStateLabel, stateComboBox);

    mainLayout->addWidget(formContainer);
    mainLayout->addSpacing(5);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setCenterButtons(true);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    mainLayout->setContentsMargins(15, 15, 15, 15);
    dialog.setFixedSize(400, 200);

    if (dialog.exec() == QDialog::Accepted) {
        return stateComboBox->currentText();
    }

    return QString();  // Return empty string if cancelled
}

bool VehicleOperations::deliverPackages(int vehicleIndex, const QString& vehicleName,
                                        int packageCount)
{
    if (packageCount <= 0) {
        QMessageBox::warning(this, "No Packages",
                             QString("Vehicle '%1' has no packages to deliver.").arg(vehicleName));
        return false;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delivery",
                                  QString("Deliver %1 package(s) from vehicle '%2'?\n\n"
                                          "This will permanently remove all packages from the vehicle.")
                                      .arg(packageCount).arg(vehicleName),
                                  QMessageBox::Yes | QMessageBox::No);

    return (reply == QMessageBox::Yes);
}

bool VehicleOperations::isVehicleOutOfService(const QString& stateText) const
{
    return stateText.contains("Out of service", Qt::CaseInsensitive);
}
