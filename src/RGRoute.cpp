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

RGRoute::RGRoute(RGMap *map, QGraphicsItem *parent) :
  RGGraphicsObjectUndo(parent),
  mMap(map),
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

  if (mMap)
  {
      QObject::connect(map, &RGMap::mapLoaded,
                       this, &RGRoute::handleMapLoaded);
  }

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
}

QRectF RGRoute::boundingRect() const
{
  return QRectF();//mBoundingRect;
}

void RGRoute::paint(QPainter * /*painter*/, const QStyleOptionGraphicsItem *, QWidget *)
{
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

void RGRoute::changePath(const QList<QPoint> &pointlist,bool canUndo)
{
  qDebug() << "RGRoute::changePath: pointlist.size():" << pointlist.size();

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

void RGRoute::handleMapLoaded(const QPixmap &/*map*/)
{
    processMapUpdate();
}

void RGRoute::setNewPoints(const QList<QPoint> &pointlist)
{
    mEditPath->setNewPoints(pointlist);
}

void RGRoute::setGeoCoordinates(const QList<QGeoCoordinate> &geoCoordinates)
{
    mGeoPath.setPath(geoCoordinates);
    processMapUpdate();
}

QGeoRectangle RGRoute::getGeoBounds() const
{
    return mGeoPath.boundingGeoRectangle();
}

QGeoPath RGRoute::getGeoPath() const
{
    return mGeoPath;
}

bool RGRoute::hasGeoBounds() const
{
    return mGeoPath.isValid();
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

void RGRoute::read(const QJsonObject &json)
{
    QJsonValue routeValue = json.value(QStringLiteral("route"));
    if (routeValue.isObject())
    {
        QJsonObject routeObject = routeValue.toObject();
        //If the loaded map has geo coordinates, it is useful to load the geoCoordinates,
        //else use the wndCoordinates
        bool geoBoundsRead = false;
        if (mMap->hasGeoBounds())
        {
            QJsonArray jsonGeoCoords = routeObject.value(QStringLiteral("geoCoordinates")).toArray();
            QList<QGeoCoordinate> geoCoordinates;
            for (const QJsonValue &geoValue: jsonGeoCoords)
            {
                QJsonObject geoObject = geoValue.toObject();
                geoCoordinates.append(QGeoCoordinate(geoObject["latitude"].toDouble(), geoObject["longitude"].toDouble()));
            }
            if (!geoCoordinates.isEmpty())
            {
                setGeoCoordinates(geoCoordinates);
                geoBoundsRead = true;
            }
        }

        if (!geoBoundsRead)
        {
            QJsonArray jsonCoords = routeObject.value(QStringLiteral("wndCoordinates")).toArray();
            QList<QPoint> pointlist;
            for (const QJsonValue &coordValue: jsonCoords)
            {
                QJsonObject coordObject = coordValue.toObject();
                pointlist.append(QPoint(coordObject["x"].toInt(), coordObject["y"].toInt()));
            }
            setNewPoints(pointlist);
        }
    }
}

void RGRoute::write(QJsonObject &json)
{
    QJsonObject routeObject;

    QJsonArray jsonGeoCoords;
    for (const QGeoCoordinate &coord: mGeoPath.path())
    {
        QJsonObject jsonCoord;
        jsonCoord.insert(QStringLiteral("longitude"), coord.longitude());
        jsonCoord.insert(QStringLiteral("latitude"), coord.latitude());
        jsonGeoCoords.append(jsonCoord);
    }
    routeObject.insert(QStringLiteral("geoCoordinates"), jsonGeoCoords);

    QJsonArray jsonCoords;
    for (const RGEditPathPoint*point: mEditPath->path())
    {
        QJsonObject jsonCoord;
        jsonCoord.insert(QStringLiteral("x"), point->x());
        jsonCoord.insert(QStringLiteral("y"), point->y());
        jsonCoords.append(jsonCoord);
    }
    routeObject.insert(QStringLiteral("wndCoordinates"), jsonCoords);

    json.insert(QStringLiteral("route"), routeObject);
}

void RGRoute::setEditMode(bool checked)
{
  mEditMode=checked;
  mEditPath->setVisible(checked);
  setCurrentFrame(mPath->countFrames()-1);
  //gives Edit path the keyboard grab :
  mEditPath->grabKeyboard();
}

void RGRoute::clearPath(bool clearGeoPath)
{
    if (clearGeoPath)
    {
        mGeoPath.clearPath();
    }
    mEditPath->clear(!clearGeoPath);
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

void RGRoute::processMapUpdate()
{
    if (mMap->hasGeoBounds() && !mGeoPath.isEmpty())
    {
        qDebug() << "RGRoute::processMapUpdate: calculating new route from geopath";
        QList<QPoint> pointlist = mMap->mapRoutePoints(mGeoPath.path());
        if (!pointlist.empty())
        {
            qDebug() << "RGRoute::processMapUpdate: setNewPoints";
            setNewPoints(pointlist);
        }
    }
}
