#ifndef VEHICLECREATOR_H
#define VEHICLECREATOR_H

#include <QWidget>
#include <QString>
#include <utility>

class VehicleCreator : public QWidget
{
    Q_OBJECT

public:
    explicit VehicleCreator(QWidget *parent = nullptr);

    std::pair<QString, double> createDrone();
    QString createVan();
    QString createRobot();

private:
    std::pair<QString, double> createVehicleDialog(const QString& title, bool needsAutonomy);
    bool isValidVehicleName(const QString& name) const;
    void showNameWarning(const QString& title, const QString& message) const;
};

#endif // VEHICLECREATOR_H
