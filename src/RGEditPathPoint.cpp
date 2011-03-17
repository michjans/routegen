#include "RGEditPathPoint.h"

RGEditPathPoint::RGEditPathPoint(QGraphicsItem *parent,QPoint point) :
    QGraphicsObject(parent),
    mMouseMove(false)
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
  qDebug()<<"mouseMove"<<mapToParent(event->pos())<<"sceneRect"<<this->scene()->sceneRect();
  qDebug()<<scene()->sceneRect().left()<<scene()->sceneRect().top()<<scene()->sceneRect().right()<<scene()->sceneRect().bottom();
  QPointF newpos=QPointF(mapToParent(event->pos()));
  if(!this->scene()->sceneRect().contains(newpos)){
    if(newpos.x() < scene()->sceneRect().left())
      newpos.setX(scene()->sceneRect().left());
    if(newpos.y() < scene()->sceneRect().top())
      newpos.setY(scene()->sceneRect().top());
    if(newpos.x() > scene()->sceneRect().right())
      newpos.setX(scene()->sceneRect().right());
    if(newpos.y() > scene()->sceneRect().bottom())
      newpos.setY(scene()->sceneRect().bottom());
  }
  setPos(newpos);
  mMouseMove=true;
  emit editMovedPoint(false);
}

void RGEditPathPoint::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  if (mMouseMove==true)
    emit editMovedPoint(true);
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
