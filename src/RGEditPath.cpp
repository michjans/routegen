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

#include "RGEditPath.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>

RGEditPath::RGEditPath(QGraphicsItem *parent) :
  QGraphicsObject(parent),
  mBoundingRect(QRectF()),
  mMousePressed(false),
  mFreeDraw(false),
  mSelect(false)
{
  setPos(0,0);
  setCursor(Qt::CrossCursor);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemHasNoContents);
}

QRectF RGEditPath::boundingRect() const
{
  return mBoundingRect;
}

void RGEditPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  Q_UNUSED(painter);
}

void RGEditPath::on_sceneRectChanged()
{
  prepareGeometryChange();
  mBoundingRect=this->scene()->sceneRect();
}

void RGEditPath::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event->button()==Qt::LeftButton && !mSelect){
    mMousePressed=true;
    addPoint(event->pos().toPoint());
    updatePointList(false); //is not undoable
  }
  //let the view manage the click for a rubberdrag
  if(event->button()==Qt::LeftButton && mSelect)
    QGraphicsItem::mousePressEvent(event);
}

void RGEditPath::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  //free draw
  //check if the left button have been pressed (event->button() does not work)
  if(mMousePressed && mFreeDraw){
    addPoint(event->pos().toPoint());
    updatePointList(false); //is not undoable
  }
}

void RGEditPath::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event->button()==Qt::LeftButton){
    mMousePressed=false;
    updatePointList();
  }
}

void RGEditPath::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
  qDebug()<<"hoverEnter";
  //gives viewWidget the focus to be able to intercept key
  this->scene()->views().at(0)->setFocus(Qt::OtherFocusReason);
}

void RGEditPath::keyPressEvent ( QKeyEvent * event )
{
  if(event->key()==Qt::Key_Shift){
    qDebug("shift pressed");
    mFreeDraw=true;
  }
  if(event->key()==Qt::Key_Control){
    qDebug("ctrl pressed");
    setCursor(Qt::ArrowCursor);
    mSelect=true;
  }
  if(event->key()==Qt::Key_Delete || event->key()==Qt::Key_Backspace){
    qDebug("dlt pressed");
    deleteSelectedPoints();
  }
}

void RGEditPath::keyReleaseEvent ( QKeyEvent * event )
{
  if(event->key()==Qt::Key_Shift){
    qDebug("shift released");
    mFreeDraw=false;
  }
  if(event->key()==Qt::Key_Control){
    qDebug("ctrl released");
    setCursor(Qt::CrossCursor);
    mSelect=false;
  }
}

void RGEditPath::clear(bool canUndo)
{
  for(int i=0;i<mEditPathPointList.size();++i){
    delete mEditPathPointList.at(i);
  }
  mEditPathPointList.clear();
  updatePointList(canUndo);
}

void RGEditPath::setNewPoints(QList<QPoint> pointlist)
{
  clear(false);
  for(int i=0;i<pointlist.size();i++){
    addPoint(pointlist.at(i));
  }
  updatePointList(false); //is not undoable
}

void RGEditPath::editPathPointMoved(bool canUndo)
{
  updatePointList(canUndo);
}

void RGEditPath::editPathPointAdd(RGEditPathPoint * point)
{
  int pointId=mEditPathPointList.indexOf(point);
  QPoint newpoint;
  if (pointId!=0)
    newpoint=(mEditPathPointList.at(pointId)->pos().toPoint()+mEditPathPointList.at(pointId-1)->pos().toPoint())/2;
  RGEditPathPoint *testpoint = new RGEditPathPoint(this,newpoint);
  mEditPathPointList.insert(pointId,testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint(bool)),this,SLOT(editPathPointMoved(bool)));
  QObject::connect(testpoint,SIGNAL(editAddPoint(RGEditPathPoint *)),this,SLOT(editPathPointAdd(RGEditPathPoint *)));

  updatePointList();
}

void RGEditPath::updatePointList(bool canUndo)
{
  QList<QPoint> pointList;
  for(int i=0;i<mEditPathPointList.size();++i){
    pointList.append(mEditPathPointList.at(i)->pos().toPoint());
  }
  emit newPointList(pointList,canUndo);
}

void RGEditPath::addPoint(QPoint point)
{
  RGEditPathPoint *testpoint=new RGEditPathPoint(this,point);
  mEditPathPointList.append(testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint(bool)),this,SLOT(editPathPointMoved(bool)));
  QObject::connect(testpoint,SIGNAL(editAddPoint(RGEditPathPoint *)),this,SLOT(editPathPointAdd(RGEditPathPoint *)));
}

void RGEditPath::deleteSelectedPoints()
{
  foreach (QGraphicsItem *item, scene()->selectedItems()) {
    RGEditPathPoint *point = qgraphicsitem_cast<RGEditPathPoint *>(item);
    if (!point)
      return;
    //remove editPoint:
    mEditPathPointList.removeAll(point);
    delete point;
    updatePointList();
  }
}
