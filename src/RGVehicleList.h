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
#include "RGVehicle.h"

class RGVehicleList
{
public:
  RGVehicleList();
  ~RGVehicleList();
  RGVehicle* getVehicle(int);
  int getCurrentVehicleId();
  void setCurrentVehicleId(int);
  int count();

private:
  QMap<int,RGVehicle*>  mMap;
  int mCurrentVehicleId;
};

#endif // RGVEHICLELIST_H
