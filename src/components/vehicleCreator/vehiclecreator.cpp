#include "VehicleCreator.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpacerItem>
#include <QSizePolicy>

VehicleCreator::VehicleCreator(QWidget *parent) : QWidget(parent) {}

std::pair<QString, double> VehicleCreator::createDrone()
{
    return createVehicleDialog("Create Drone", true);
}

QString VehicleCreator::createVan()
{
    auto result = createVehicleDialog("Create Van", false);
    return result.first;
}

QString VehicleCreator::createRobot()
{
    auto result = createVehicleDialog("Create Robot", false);
    return result.first;
}

std::pair<QString, double> VehicleCreator::createVehicleDialog(const QString& title, bool needsAutonomy)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setFixedSize(400, needsAutonomy ? 160 : 120);

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
        QLineEdit, QDoubleSpinBox {
            border: 1px solid #bdc3c7;
            border-radius: 3px;
            padding: 5px;
            font-size: 10pt;
            background-color: #ffffff;
            min-height: 20px;
        }
        QLineEdit:focus, QDoubleSpinBox:focus {
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
        QPushButton:disabled {
            background-color: #ecf0f1;
            color: #95a5a6;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // Create a container widget for the form to control spacing better
    QWidget *formContainer = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formContainer);
    formLayout->setSpacing(10);  // Control spacing between form rows
    formLayout->setContentsMargins(0, 0, 0, 0);  // Remove default margins

    // Vehicle name input
    QLabel *nameLabel = new QLabel("Vehicle Name:");
    nameLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Enter vehicle name");

    formLayout->addRow(nameLabel, nameEdit);

    // Autonomy input (only for drones)
    QDoubleSpinBox *autonomySpinBox = nullptr;
    if (needsAutonomy) {
        QLabel *autonomyLabel = new QLabel("Autonomy (km):");
        autonomyLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

        autonomySpinBox = new QDoubleSpinBox();
        autonomySpinBox->setRange(-999999999.0, 999999999.0);
        autonomySpinBox->setValue(10.0);
        autonomySpinBox->setSingleStep(1.0);
        autonomySpinBox->setSuffix(" km");
        formLayout->addRow(autonomyLabel, autonomySpinBox);
    }

    // Add form container to main layout
    mainLayout->addWidget(formContainer);

    // Button box with controlled margins
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setCenterButtons(true);
    buttonBox->setContentsMargins(0, 0, 0, 0);  // Remove default margins

    // Connect button signals
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    mainLayout->addWidget(buttonBox);

    mainLayout->setContentsMargins(15, 15, 15, 15);  // Dialog margins

    // Validate input when OK is clicked
    dialog.setLayout(mainLayout);

    if (dialog.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();

        if (!isValidVehicleName(name)) {
            return std::make_pair(QString(), 0.0);
        }

        if (needsAutonomy && autonomySpinBox) {
            double autonomy = autonomySpinBox->value();

            // Frontend validation for drone autonomy
            if (autonomy <= 0) {
                QMessageBox::warning(this, "Invalid Input",
                                     "Drone autonomy must be positive.");
                return std::make_pair(QString(), 0.0);
            }

            return std::make_pair(name, autonomy);
        } else {
            return std::make_pair(name, 0.0);
        }
    }

    return std::make_pair(QString(), 0.0);
}

bool VehicleCreator::isValidVehicleName(const QString& name) const
{
    if (name.isEmpty()) {
        showNameWarning("Invalid Name", "Vehicle name cannot be empty.");
        return false;
    }
    return true;
}

void VehicleCreator::showNameWarning(const QString& title, const QString& message) const
{
    QMessageBox::warning(const_cast<QWidget*>(this->parentWidget()), title, message);
}


