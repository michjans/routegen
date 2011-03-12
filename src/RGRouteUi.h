#ifndef RGROUTEUI_H
#define RGROUTEUI_H

#include <QWidget>
#include "RGVehicleList.h"

namespace Ui {
  class routeUi;
}

class RGRouteUi : public QWidget
{
  Q_OBJECT

public:
  explicit RGRouteUi(QWidget *parent = 0);
  ~RGRouteUi();
  void setVehicleList(RGVehicleList *vehicleList);
  void init();

public slots:
  void on_playbackChanged(bool);

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
