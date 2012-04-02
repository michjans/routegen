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

#include <QDir>
#include <QDebug>
#include <QDesktopServices>

#include "RGSettings.h"
#include "RGVehicleList.h"

RGVehicleList::RGVehicleList()
{
  QDir vehicleDir = QDir::currentPath() + "/vehicles";
  QStringList filters;
  filters << "*.bmp" << "*.gif" << "*.png" << "*.jpg" << "*.tif";
  vehicleDir.setNameFilters(filters);
  QFileInfoList vehicles = vehicleDir.entryInfoList();

  //Also look in user's local data directory
  vehicleDir.setPath(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/vehicles");
  if (vehicleDir.exists()) {
    vehicles.append(vehicleDir.entryInfoList());
  }

  //check folder ~/.routegen/vehicles in linux
#ifdef Q_WS_X11
  vehicleDir.mkpath(QDir::homePath() + "/.routegen/vehicles");
  vehicleDir.setPath( QDir::homePath() + "/.routegen/vehicles");
  vehicles.append(vehicleDir.entryInfoList());
#endif

  RGVehicle *vehicle;
  vehicle= new RGVehicle();
  mMap.insert(0,vehicle);
  mCurrentVehicleId=0;
  int i=1;
  for (QFileInfoList::iterator it = vehicles.begin(); it != vehicles.end(); it++){
    vehicle= new RGVehicle(it->absoluteFilePath(),RGSettings::getVehicleSize(it->baseName()),
                           RGSettings::getVehicleMirrored(it->baseName()),
                           RGSettings::getVehicleAngle(it->baseName()),
                           RGSettings::getVehicleAcceptsRotation(it->baseName()),
                           RGSettings::getVehicleOrigin(it->baseName()));
    if (vehicle->getRawSize()==0){
      delete vehicle;
      continue;
    }
    mMap.insert(i,vehicle);
    if(it->baseName()==RGSettings::getLastVehicleName())
      mCurrentVehicleId=i;
    i++;
  }
}

RGVehicleList::~RGVehicleList()
{
  qDebug()<<"RGVehicleList::~RGVehicleList()";
}

RGVehicle * RGVehicleList::getVehicle(int idx)
{
  if(mMap.count()>idx)
    return mMap.value(idx);
  return new RGVehicle();
}

RGVehicle * RGVehicleList::getCurrentVehicle()
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
  mMap.value(mCurrentVehicleId)->setParentItem(NULL);
  mMap.value(mCurrentVehicleId)->setVisible(false);

  mCurrentVehicleId=idx;
  RGSettings::setLastVehicleName(mMap.value(idx)->getName());
}

int RGVehicleList::count()
{
  return mMap.count();
}

void RGVehicleList::saveVehiclesSettings()
{
  for (int i=1;i<mMap.count();i++){
    RGSettings::setVehicleAngle(mMap.value(i)->getName(),mMap.value(i)->getStartAngle());
    RGSettings::setVehicleSize(mMap.value(i)->getName(),mMap.value(i)->getSize());
    RGSettings::setVehicleMirrored(mMap.value(i)->getName(),mMap.value(i)->getMirror());
    RGSettings::setVehicleOrigin(mMap.value(i)->getName(),mMap.value(i)->getOrigin());
    RGSettings::setVehicleAcceptsRotation(mMap.value(i)->getName(),mMap.value(i)->acceptsRotation());
  }
}

void RGVehicleList::loadVehiclesSettings()
{
  for (int i=1;i<mMap.count();i++){
    mMap.value(i)->setStartAngle(RGSettings::getVehicleAngle(mMap.value(i)->getName()));
    mMap.value(i)->setSize(RGSettings::getVehicleSize(mMap.value(i)->getName()));
    mMap.value(i)->setMirror(RGSettings::getVehicleMirrored(mMap.value(i)->getName()));
    mMap.value(i)->setOrigin(RGSettings::getVehicleOrigin(mMap.value(i)->getName()));
    mMap.value(i)->acceptRotation(RGSettings::getVehicleAcceptsRotation(mMap.value(i)->getName()));
  }
}
