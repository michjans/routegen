/*
    Copyright (C) 2008  Michiel Jansen

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
#include <QList>
#include "RGVehicle.h"

class RGVehicleDialog : public QDialog
{
    Q_OBJECT

public:
    /**
   * Creates a new RGVehicleDialog object.
   */
    RGVehicleDialog(QWidget *parent);
    ~RGVehicleDialog();
    RGVehicle getVehicle();
    int count();

public slots:
    void accept();

private  slots:
    void on_vehicleListWidget_currentRowChanged(int);
    void on_sizeSB_valueChanged(int);
    void on_angleSB_valueChanged(int);
    void on_resetSizePB_clicked(bool);
    void on_resetAnglePB_clicked(bool);
    void on_mirrorCB_toggled(bool);

private:
    void updateVehiclePreview();

    Ui_vehicleDialog ui;
    QList<RGVehicle*>  mList;
    int mVehicleId;
};

#endif
