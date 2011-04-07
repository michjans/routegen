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

#include "RGEditPathPoint.h"

RGEditPathPoint::RGEditPathPoint(QGraphicsItem *parent,QPoint point) :
  QGraphicsObject(parent),
  mMouseMove(false)
{
  setCursor(Qt::SizeAllCursor);
  //setAcceptsHoverEvents(true);
  setPos(point);
  setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
  scene()->clearSelection();
  setSelected(true);
}


QRectF RGEditPathPoint::boundingRect() const
{
  return QRectF(-5,-5,10,10);
}

void RGEditPathPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setBrush(Qt::black);
  if(isSelected())
    painter->setBrush(Qt::red);
  painter->drawRect(this->boundingRect());
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
  mMouseMove=true;
  //send signal to redraw path,don't stock in undo:
  emit editMovedPoint(false);
  QGraphicsItem::mouseMoveEvent(event);
  setPos(newpos);
}

void RGEditPathPoint::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  if (mMouseMove==true)
    //send signal to redraw path,stock in undo:
    emit editMovedPoint(true);
  QGraphicsItem::mouseReleaseEvent(event);
}

void RGEditPathPoint::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug()<<"DbleClick on edit point";
  emit editAddPoint(this);
}
