#include "RGEditPathPoint.h"

RGEditPathPoint::RGEditPathPoint(QGraphicsItem *parent,QPoint point) :
    QGraphicsObject(parent)
{
  setCursor(Qt::SizeAllCursor);
  //setAcceptsHoverEvents(true);
  setPos(point);
}


QRectF RGEditPathPoint::boundingRect() const
{
     return QRectF(-5,-5,10,10);
 }

void RGEditPathPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setBrush(Qt::black);
  painter->drawRect(this->boundingRect());
}

void RGEditPathPoint::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event->buttons()==Qt::RightButton){
    qDebug()<<"rightClick on edit point";
    emit editDelPoint(this);
  }

}

void RGEditPathPoint::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  qDebug()<<"mouseMove"<<QString::number(mapToParent(event->pos()).x()) + "," + QString::number(mapToParent(event->pos()).y());
  setPos(mapToParent(event->pos()));
  emit editMovedPoint();
}

void RGEditPathPoint::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug()<<"DbleClick on edit point";
  emit editAddPoint(this);
}

/*void RGEditPathPoint::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
 qDebug()<<"hoverEnter";
 setCursor(Qt::ForbiddenCursor);
}

void RGEditPathPoint::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
  qDebug()<<"hoverLeave";
  setCursor(Qt::SizeAllCursor);
}*/
