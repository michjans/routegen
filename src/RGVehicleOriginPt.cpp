#include "RGVehicleOriginPt.h"

#include "RGVehicle.h"

RGVehicleOriginPt::RGVehicleOriginPt(QGraphicsItem *parent) :
    QGraphicsItem (parent)
{
  setPos(0,0);
  setCursor(Qt::CrossCursor);
}

QRectF RGVehicleOriginPt::boundingRect() const
{
  return this->parentItem()->boundingRect();
}

void RGVehicleOriginPt::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{

}

void RGVehicleOriginPt::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  RGVehicle *vehicle = qgraphicsitem_cast<RGVehicle *>(this->parentItem());
  qDebug()<<"new Origin"<<event->pos()-this->boundingRect().topLeft();
  vehicle->setOrigin(event->pos()-this->boundingRect().topLeft());
}

void RGVehicleOriginPt::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  //setOrigin(event->pos());
}
