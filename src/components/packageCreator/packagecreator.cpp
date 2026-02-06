#include "PackageCreator.h"
#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <stdexcept>

PackageCreator::PackageCreator(QWidget *parent) : QWidget(parent) {}

std::unique_ptr<Package> PackageCreator::createStandardPackage()
{
    return createPackageDialog("Create Standard Package", PackageType::Standard);
}

std::unique_ptr<Package> PackageCreator::createFragilePackage()
{
    return createPackageDialog("Create Fragile Package", PackageType::Fragile);
}

std::unique_ptr<Package> PackageCreator::createRefrigeratedPackage()
{
    return createPackageDialog("Create Refrigerated Package", PackageType::Refrigerated);
}

std::unique_ptr<Package> PackageCreator::createExpressPackage()
{
    return createPackageDialog("Create Express Package", PackageType::Express);
}

std::unique_ptr<Package> PackageCreator::createPackageDialog(const QString& title, PackageType type)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);

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
        QLineEdit, QDoubleSpinBox, QComboBox {
            border: 1px solid #bdc3c7;
            border-radius: 3px;
            padding: 5px;
            font-size: 10pt;
            background-color: #ffffff;
            min-height: 20px;
        }
        QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {
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

    QLabel *descLabel = new QLabel("Description:");
    descLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
    QLineEdit *descEdit = new QLineEdit();
    descEdit->setPlaceholderText("Enter package description");
    descEdit->setMinimumHeight(30);
    formLayout->addRow(descLabel, descEdit);

    QLabel *weightLabel = new QLabel("Weight (g):");
    weightLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
    QDoubleSpinBox *weightSpinBox = new QDoubleSpinBox();
    weightSpinBox->setRange(-999999999.0, 999999999.0); 
    weightSpinBox->setValue(100.0);
    weightSpinBox->setSingleStep(10.0);
    weightSpinBox->setSuffix(" g");
    weightSpinBox->setMinimumHeight(30);
    formLayout->addRow(weightLabel, weightSpinBox);

    QComboBox *fragilityComboBox = nullptr;
    QDoubleSpinBox *temperatureSpinBox = nullptr;

    if (type == PackageType::Fragile) {
        QLabel *fragilityLabel = new QLabel("Fragility Level:");
        fragilityLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
        fragilityComboBox = new QComboBox();
        fragilityComboBox->addItems({"LOW", "MEDIUM", "HIGH"});
        fragilityComboBox->setMinimumHeight(30);
        formLayout->addRow(fragilityLabel, fragilityComboBox);
        dialog.setFixedSize(400, 200);
    }
    else if (type == PackageType::Refrigerated) {
        QLabel *tempLabel = new QLabel("Temperature (°C):");
        tempLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
        temperatureSpinBox = new QDoubleSpinBox();
        temperatureSpinBox->setRange(-50.0, 10.0); 
        temperatureSpinBox->setValue(4.0);
        temperatureSpinBox->setSingleStep(0.5);
        temperatureSpinBox->setSuffix(" °C");
        temperatureSpinBox->setMinimumHeight(30);
        formLayout->addRow(tempLabel, temperatureSpinBox);
        dialog.setFixedSize(400, 200);
    }
    else {
        dialog.setFixedSize(400, 170);
    }

    mainLayout->addWidget(formContainer);
    mainLayout->addSpacing(5);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->setCenterButtons(true);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    mainLayout->setContentsMargins(15, 15, 15, 15);

    if (dialog.exec() == QDialog::Accepted) {
        QString desc = descEdit->text().trimmed();
        double weight = weightSpinBox->value();

        // Frontend validation for description
        if (desc.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Description cannot be empty.");
            return nullptr;
        }

        // Frontend validation for weight
        if (weight <= 0) {
            QMessageBox::warning(this, "Invalid Input", "Package weight must be positive.");
            return nullptr;
        }

        try {
            switch (type) {
            case PackageType::Standard:
                return std::make_unique<StandardPackage>(desc.toStdString(), weight);
            case PackageType::Fragile:
                if (fragilityComboBox) {
                    QString level = fragilityComboBox->currentText();
                    FragilityLevel fragility = FragilityLevel::LOW;
                    if (level == "MEDIUM") fragility = FragilityLevel::MEDIUM;
                    else if (level == "HIGH") fragility = FragilityLevel::HIGH;
                    return std::make_unique<FragilePackage>(desc.toStdString(), weight, fragility);
                }
                break;
            case PackageType::Refrigerated:
                if (temperatureSpinBox) {
                    double temp = temperatureSpinBox->value();
                    // Frontend validation for temperature range
                    if (temp < -50.0 || temp > 10.0) {
                        QMessageBox::warning(this, "Invalid Input",
                                             "Refrigerated package temperature must be between -50°C and +10°C.");
                        return nullptr;
                    }
                    return std::make_unique<RefrigeratedPackage>(desc.toStdString(), weight, temp);
                }
                break;
            case PackageType::Express:
                return std::make_unique<ExpressPackage>(desc.toStdString(), weight);
            }
        } catch (const std::invalid_argument& e) {
            // Catch backend validation exceptions and show them to the user
            QMessageBox::critical(this, "Package Creation Error",
                                  QString("Failed to create package: %1").arg(e.what()));
            return nullptr;
        } catch (const std::exception& e) {
            // Catch any other exceptions
            QMessageBox::critical(this, "Package Creation Error",
                                  QString("Unexpected error: %1").arg(e.what()));
            return nullptr;
        }
    }

    return nullptr;
}


