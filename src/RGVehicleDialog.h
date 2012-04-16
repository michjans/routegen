/*
    Copyright (C) 2008-2011  Michiel Jansen
    Copyright (C) 2010-2011  Fabien Valthier

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

#ifndef RG_VEHICLELIST_H
#define RG_VEHICLELIST_H

#include "ui_vehicledialog.h"
#include <QString>
#include <QTimer>
#include <QPen>
#include "RGVehicle.h"
#include "RGVehicleList.h"
#include "RGVehicleOriginPt.h"
#include <QGraphicsScene>

class RGVehicleDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * Creates a new RGVehicleDialog object.
   */
  RGVehicleDialog(QWidget *parent,RGVehicleList *vehicleList,const QPen &pen);
  ~RGVehicleDialog();

public slots:
  void accept();
  void reject();

private  slots:
  void on_addVehiclePB_clicked(bool);
  void on_removeVehiclePB_clicked(bool);
  void on_vehicleListWidget_currentRowChanged(int);
  void on_sizeSB_valueChanged(int);
  void on_angleSlider_valueChanged(int);
  void on_resetSizePB_clicked(bool);
  void on_mirrorCB_toggled(bool);
  void on_rotateCB_toggled(bool);
  void playTimerEvent();

private:

  Ui_vehicleDialog ui;
  RGVehicleList *mVehicleList;
  RGVehicleOriginPt *mVehicleOrigin;
  QTimer *mPlayTimer;
  int mTimerCounter;
  QGraphicsScene *mScene;
};

#endif
