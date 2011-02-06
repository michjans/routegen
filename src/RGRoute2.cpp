#include "RGRoute2.h"

RGRoute2::RGRoute2(QObject *parent) :
    QGraphicsItem()
{
  mRouteUi = new RGRouteUi();

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
  mRouteUi->setVehicleList(mVehicleList);
  //QObject::connect(mRouteUi,SIGNAL((QList<QPoint>)),this,SLOT(getNewPointList(QList<QPoint>)));
}

QRectF RGRoute2::boundingRect() const
{
     return QRectF(0,0,0,0);
 }

void RGRoute2::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  //painter->setBrush(Qt::black);
  //painter->drawRect(-10,-10,20,20);
}

QWidget *RGRoute2::widgetSettings()
{
  return mRouteUi;
}
