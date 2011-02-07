#include "RGRoute2.h"

#include <QDebug>

RGRoute2::RGRoute2(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
  mPath=new RGPath(this);
  mEditPath=new RGEditPath(this);
  QObject::connect(mEditPath,SIGNAL(newPointList(QList<QPoint>)),mPath,SLOT(newPointList(QList<QPoint>)));

  mRouteUi = new RGRouteUi();
  QObject::connect(mRouteUi,SIGNAL(penChanged(const QPen &)),this,SLOT(on_penChanged(const QPen &)));
  QObject::connect(mRouteUi,SIGNAL(smoothPathChecked(bool)),this,SLOT(on_smoothPathChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(totalTimeChecked(bool)),this,SLOT(on_totalTimeChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(routeTimeChanged(int)),this,SLOT(on_routeTimeChanged(int)));
  QObject::connect(mRouteUi,SIGNAL(vehicleChanged(int)),this,SLOT(on_vehicleChanged(int)));

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
  mRouteUi->setVehicleList(mVehicleList);

  //set initial by sending signals
  mRouteUi->init();
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

void RGRoute2::on_penChanged(const QPen & pen)
{
  qDebug()<<"on_penChanged";
  mPath->setPen(pen);
}

void RGRoute2::on_totalTimeChecked(bool checked)
{
  qDebug()<<"on_setTotalTimeChecked";
}
void RGRoute2::on_smoothPathChecked(bool checked)
{
  qDebug()<<"on_smoothPathChecked";
}

void RGRoute2::on_routeTimeChanged(int time)
{
  qDebug()<<"on_routeTimeChanged";
}

void RGRoute2::on_vehicleChanged(int index)
{
  qDebug()<<"on_vehicleChanged";
  mVehicleList->setCurrentVehicleId(index);
  //mRGMapWidget->setVehicle(*mVehicleList->getVehicle(index));
}
