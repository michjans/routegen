/*
    Copyright (C) 2011  Michiel Jansen
    Copyright (C) 2011  Fabien Valthier

  This file is part of Route Generator.

    Route Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "RGSettings.h"
#include "RGVehicleList.h"

RGVehicleList::RGVehicleList()
{
    QDir vehicleDir = QDir::currentPath() + "/vehicles";
    QStringList filters;
    filters << QStringLiteral("*.bmp") << QStringLiteral("*.gif") << QStringLiteral("*.png") << QStringLiteral("*.jpg") << QStringLiteral("*.svg");
    vehicleDir.setNameFilters(filters);
    QFileInfoList vehicles = vehicleDir.entryInfoList();

    //TODO: Check on Windows if this really is the correct location or should we use AppLocalDataLocation?
#ifdef Q_OS_WIN
    Force compiler error so we can check this !
#endif
        vehicleDir.setPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/vehicles");
    if (vehicleDir.exists())
    {
        vehicles.append(vehicleDir.entryInfoList());
    }
    else
    {
        //Create folder for custom vehicles
        vehicleDir.mkpath(vehicleDir.absolutePath());
    }

    //Always add 'none' vehicle
    mMap.insert(0, new RGVehicle());
    mCurrentVehicleId = 0;
    int i = 1;
    for (QFileInfoList::iterator it = vehicles.begin(); it != vehicles.end(); it++)
    {
        //Add vehicle (and mark vehicles that reside in custom vehicle path as custom)
        if (addVehicle(*it, it->absoluteFilePath().startsWith(vehicleDir.absolutePath())) == nullptr)
            continue;
        if (it->baseName() == RGSettings::getLastVehicleName())
            mCurrentVehicleId = i;
        i++;
    }
}

RGVehicleList::~RGVehicleList()
{
    qDebug() << "RGVehicleList::~RGVehicleList()";
    for (int i = 1; i < mMap.count(); i++)
    {
        delete mMap.value(i);
    }
    mMap.clear();
}

RGVehicle* RGVehicleList::getVehicle(int idx)
{
    if (mMap.count() > idx)
        return mMap.value(idx);
    return new RGVehicle();
}

RGVehicle* RGVehicleList::getCurrentVehicle()
{
    return mMap.value(mCurrentVehicleId);
}

int RGVehicleList::getCurrentVehicleId()
{
    return mCurrentVehicleId;
}

void RGVehicleList::setCurrentVehicleId(int idx)
{
    //set no parent:
    mMap.value(mCurrentVehicleId)->setParentItem(nullptr);
    mMap.value(mCurrentVehicleId)->setVisible(false);

    mCurrentVehicleId = idx;
    RGSettings::setLastVehicleName(mMap.value(idx)->getName());
}

int RGVehicleList::count()
{
    return mMap.count();
}

void RGVehicleList::saveVehiclesSettings()
{
    for (int i = 1; i < mMap.count(); i++)
    {
        RGSettings::setVehicleAngle(mMap.value(i)->getName(), mMap.value(i)->getStartAngle());
        RGSettings::setVehicleSize(mMap.value(i)->getName(), mMap.value(i)->getSize());
        RGSettings::setVehicleMirrored(mMap.value(i)->getName(), mMap.value(i)->getMirror());
        RGSettings::setVehicleOrigin(mMap.value(i)->getName(), mMap.value(i)->getOrigin());
        RGSettings::setVehicleAcceptsRotation(mMap.value(i)->getName(), mMap.value(i)->acceptsRotation());
    }
}

void RGVehicleList::loadVehiclesSettings()
{
    for (int i = 1; i < mMap.count(); i++)
    {
        mMap.value(i)->setStartAngle(RGSettings::getVehicleAngle(mMap.value(i)->getName()));
        mMap.value(i)->setSize(RGSettings::getVehicleSize(mMap.value(i)->getName()));
        mMap.value(i)->setMirror(RGSettings::getVehicleMirrored(mMap.value(i)->getName()));
        mMap.value(i)->setOrigin(RGSettings::getVehicleOrigin(mMap.value(i)->getName()));
        mMap.value(i)->acceptRotation(RGSettings::getVehicleAcceptsRotation(mMap.value(i)->getName()));
    }
}

RGVehicle* RGVehicleList::addCustomVehicle(const QString& fileName, QString& errStr)
{
    //TODO: Check on Windows if this really is the correct location or should we use AppLocalDataLocation?
#ifdef Q_OS_WIN
    Force compiler error so we can check this !
#endif
        QDir customVehicleDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/vehicles";
    if (!customVehicleDir.exists())
    {
        //It should have been created in constructor, so it's an error if this didn't succeed
        errStr = QObject::tr("Unable to find folder for custom vehicles.");
        return nullptr;
    }

    QFile orgFile(fileName);
    QFileInfo destFile(customVehicleDir.absolutePath() + "/" + QFileInfo(orgFile).fileName());
    if (!orgFile.copy(destFile.absoluteFilePath()))
    {
        errStr = QObject::tr("Unable to copy custom vehicle to folder for custom vehicles.");
        return nullptr;
    }

    RGVehicle* vehicle = addVehicle(destFile, true);
    if (vehicle == nullptr)
        errStr = QObject::tr("Error adding vehicle (unexpected file format or error in file?)");

    return vehicle;
}

RGVehicle* RGVehicleList::addVehicle(const QFileInfo& destFile, bool custom)
{
    RGVehicle* vehicle = new RGVehicle(destFile.absoluteFilePath(), RGSettings::getVehicleSize(destFile.baseName()),
                                       RGSettings::getVehicleMirrored(destFile.baseName()), RGSettings::getVehicleAngle(destFile.baseName()),
                                       RGSettings::getVehicleAcceptsRotation(destFile.baseName()), RGSettings::getVehicleOrigin(destFile.baseName()));
    if (vehicle->getRawSize() == 0)
    {
        delete vehicle;
        return nullptr;
    }
    vehicle->setIsCustom(custom);
    mMap.insert(count(), vehicle);
    return vehicle;
}

void RGVehicleList::removeCustomVehicle(RGVehicle* vehicle)
{
    int idx;
    for (idx = 1; idx < mMap.count(); idx++)
    {
        if (vehicle == mMap.value(idx))
        {
            mMap.erase(mMap.find(idx));
            break;
        }
    }
    QFile::remove(vehicle->getFileName());
    delete vehicle;

    if (mCurrentVehicleId == idx)
    {
        mCurrentVehicleId--; //Make previous current
        RGSettings::setLastVehicleName(mMap.value(mCurrentVehicleId)->getName());
    }
}
