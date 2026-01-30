#ifndef PACKAGECREATOR_H
#define PACKAGECREATOR_H

#include <QWidget>
#include <memory>
#include "../../core/Package.h"

enum class PackageType {
    Standard,
    Fragile,
    Refrigerated,
    Express
};

class PackageCreator : public QWidget
{
    Q_OBJECT

public:
    explicit PackageCreator(QWidget *parent = nullptr);

    std::unique_ptr<Package> createStandardPackage();
    std::unique_ptr<Package> createFragilePackage();
    std::unique_ptr<Package> createRefrigeratedPackage();
    std::unique_ptr<Package> createExpressPackage();

private:
    std::unique_ptr<Package> createPackageDialog(const QString& title, PackageType type);
};

#endif // PACKAGECREATOR_H
