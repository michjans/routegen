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

private slots:
  void on_vehicleSettingsPB_clicked(bool);

signals:
  void penChanged(const QColor &color,int size,Qt::PenStyle style);

private:
  QIcon createIconForStyle(Qt::PenStyle);
  void setPen();

private:
  Ui::routeUi *ui;
  RGVehicleList     *mVehicleList;
};

#endif // RGROUTEUI_H
