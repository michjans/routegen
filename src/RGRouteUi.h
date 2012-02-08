/*
    Copyright (C) 2009-2011  Michiel Jansen
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

#ifndef RGROUTEUI_H
#define RGROUTEUI_H

#include <QWidget>

namespace Ui {
class routeUi;
}
class RGVehicleList;

/**
 * RGRouteUi is the toolbar to control the route (route width, duration, playback, etc.)
 */
class RGRouteUi : public QWidget
{
  Q_OBJECT

public:
  explicit RGRouteUi(QWidget *parent = 0);
  ~RGRouteUi();
  void setVehicleList(RGVehicleList *vehicleList);
  void init();

public slots:
  void blockEssentialControls(bool);

private slots:
  void on_vehicleSettingsPB_clicked(bool);
  void on_penSizeSB_valueChanged(int size);
  void on_lineStyleCB_activated(int idx);
  void on_totalTimeCB_toggled(bool checked);
  void on_smoothPathCB_toggled(bool checked);
  void on_routeTimeSB_valueChanged(int time);
  void on_vehicleCB_currentIndexChanged(int index);
  void on_routeColorPB_clicked(bool);

signals:
  void penChanged(const QPen &);
  void totalTimeChecked(bool);
  void smoothPathChecked(bool);
  void routeTimeChanged(int);
  void vehicleChanged();

private:
  QIcon createIconForStyle(Qt::PenStyle);
  void setPen();

private:
  Ui::routeUi *ui;
  RGVehicleList     *mVehicleList;
};

#endif // RGROUTEUI_H
