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

#ifndef RGVEHICLELIST_H
#define RGVEHICLELIST_H

#include <QMap>
#include <QFileInfo>
#include "RGVehicle.h"

class RGVehicleList
{
public:
  RGVehicleList();
  ~RGVehicleList();
  RGVehicle* getVehicle(int);
  RGVehicle * getCurrentVehicle();
  int getCurrentVehicleId();
  void setCurrentVehicleId(int);
  int count();
  void saveVehiclesSettings();
  void loadVehiclesSettings();

  /**
   * Adds a custom vehicle.
   * @param  fileName the full path to the icon image to be added
   * @param  errStr if vehicle coudn't be added, contains the error message
   * @return the added vehicle or nullptr if failed
   */
  RGVehicle* addCustomVehicle(const QString &fileName, QString &errStr);

  /**
   * Removes and also physically deletes the custom vehicle.
   * The vehicle object will also be deleted!
   */
  void       removeCustomVehicle(RGVehicle *vehicle);


private:
  RGVehicle* addVehicle(const QFileInfo &fileName, bool isCustom);

  QMap<int,RGVehicle*>  mMap;
  int mCurrentVehicleId;
};

#endif // RGVEHICLELIST_H
