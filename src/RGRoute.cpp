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

#include "RGRoute.h"
#include <QDebug>

RGRoute::RGRoute(QGraphicsItem *parent) :
  RGGraphicsObjectUndo(parent),
  mIconlessBeginEndFrames(false),
  mPlayback(false),
  mEditMode(false)
{
  mPath=new RGPath(this);
  mEditPath=new RGEditPath(this);
  mEditPath->setVisible(false);
  QObject::connect(mEditPath,SIGNAL(newPointList(QList<QPoint>,bool)),this,SLOT(changePath(QList<QPoint>,bool)));
  QObject::connect(this,SIGNAL(sceneRectChanged()),mEditPath,SLOT(on_sceneRectChanged()));
  QObject::connect(this,SIGNAL(sceneRectChanged()),this,SLOT(clearPath()));

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
}

QRectF RGRoute::boundingRect() const
{
  return QRectF();//mBoundingRect;
}

void RGRoute::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  Q_UNUSED(painter);
}

void RGRoute::setSmoothCoef(int dsmooth)
{
  mPath->setSmoothCoef(dsmooth);
  setCurrentFrame(mPath->countFrames()-1);
}

void RGRoute::startPlayback(bool play)
{
  mPlayback=play;
}

void RGRoute::changePen(const QPen & pen)
{
  mPath->setPen(pen);
}

void RGRoute::activateTotalTime(bool checked)
{
  if (checked)
    mPath->setPlayMode(1);
  else
    mPath->setPlayMode(0);
}

void RGRoute::activateSmoothPath(bool checked)
{
  mPath->setSmoothPath(checked);
  //if playback is on, update to the current frame else to the last :
  if(mPlayback)
    setCurrentFrame(mPath->getCurrentFrame());
  else
    setCurrentFrame(mPath->countFrames()-1);
}

void RGRoute::setRouteTime(int time)
{
  mPath->setTotalTime(time);
}

void RGRoute::handleVehicleChange()
{
  mVehicleList->getCurrentVehicle()->setParentItem(this);
  updateVehicle();
}

void RGRoute::changePath(QList<QPoint> pointlist,bool canUndo)
{
  mPath->newPointList(pointlist);
  updateVehicle();

  if(pointlist.size()>=2)
    emit canGenerate(true);
  if(pointlist.size()<2)
    emit canGenerate(false);

  //send Undo data
  if(canUndo){
    QList<QVariant> vlist;
    for(int i=0;i<pointlist.size();i++){
      vlist.append(QVariant(pointlist.at(i)));
    }
    QVariant data(vlist);
    emit newUndoable(this,data);
  }
}

void RGRoute::setNewPoints(const QList<QPoint> &pointlist)
{
    mEditPath->setNewPoints(pointlist);
}

void RGRoute::setNewWorldCoordinates(const QList<QGeoCoordinate> &geoCoordinates)
{
    //TODO: Introduce RGMap with realworld bounds method and store pointer in RGRoute
    if (mRealWorldBounds.isValid())
    {
        //TODO: Put this in separate method, so that once a valid map boundary is set
        //      we call this same method
        QList<QPoint> pointlist;
        double xScale = mWidth / mRealWorldBounds.width();
        double yScale = mHeight / mRealWorldBounds.height();

        for (const QGeoCoordinate &coord: geoCoordinates)
        {
            QPoint point((coord.longitude() - mRealWorldBounds.x()) * xScale,
                         (mRealWorldBounds.y() - coord.latitude()) * yScale);
            pointlist.append(point);
        }
        mEditPath->setNewPoints(pointlist);
    }
    else
    {
        //TODO: No map boundaries known, yet, store the coordinates
    }
    mGeoPath.setPath(geoCoordinates);
}

void RGRoute::setRealWorldMapping(const QRectF &mapBounds, int width, int height)
{
    mRealWorldBounds = mapBounds;
    mWidth = width;
    mHeight = height;
}

const QRectF RGRoute::getRealWorldBounds() const
{
    return mRealWorldBounds;
}

void RGRoute::undoredo(QVariant data)
{
  QList<QVariant> vlist=data.toList();
  QList<QPoint> pointlist;
  for(int i=0;i<vlist.size();i++){
    pointlist.append(vlist.at(i).toPoint());
  }
  setNewPoints(pointlist);
}

void RGRoute::setEditMode(bool checked)
{
  mEditMode=checked;
  mEditPath->setVisible(checked);
  setCurrentFrame(mPath->countFrames()-1);
  //gives Edit path the keyboard grab :
  mEditPath->grabKeyboard();
}

void RGRoute::clearPath()
{
  mEditPath->clear();
  updateVehicle();
}

int RGRoute::countFrames()
{
  return mPath->countFrames();
}

void RGRoute::setCurrentFrame(int frame)
{
  mPath->setCurrentFrame(frame);
  updateVehicle();
}

void RGRoute::setIconlessBeginEndFrames(bool val)
{
  mIconlessBeginEndFrames=val;
  updateVehicle();
}

void RGRoute::updateVehicle()
{
  //if in edit mode
  if(mEditMode){
    mVehicleList->getCurrentVehicle()->setVisible(false);
    return;
  }
  int frame=mPath->getCurrentFrame();
  //if just one point
  if(countFrames()<2){
    mVehicleList->getCurrentVehicle()->setVisible(false);
    return;
  }
  mVehicleList->getCurrentVehicle()->setTime(mPath->getCurrentTime());
  mVehicleList->getCurrentVehicle()->setPos(mPath->getEndPos());
  mVehicleList->getCurrentVehicle()->setRotation(mPath->getAngle());
  if(mIconlessBeginEndFrames && (frame==0 || frame==countFrames()-1))
    mVehicleList->getCurrentVehicle()->setVisible(false);
  else
    mVehicleList->getCurrentVehicle()->setVisible(true);
}
