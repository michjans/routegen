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

#ifndef RGROUTE_H
#define RGROUTE_H

#include "RGGraphicsObjectUndo.h"
#include "RGMap.h"
#include "RGVehicleList.h"
#include "RGPath.h"
#include "RGEditPath.h"

#include <QGeoCoordinate>
#include <QGeoPath>
#include <QGeoRectangle>


class RGRoute : public RGGraphicsObjectUndo
{
  Q_OBJECT
public:
  explicit RGRoute(RGMap *map, QGraphicsItem *parent = 0);
  QRectF boundingRect() const;
  RGVehicleList *getVehicleList() const {return mVehicleList;}
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
  void setSmoothCoef(int);
  void setEditMode(bool);
  int countFrames();
  void setCurrentFrame(int);
  void setIconlessBeginEndFrames(bool);
  void setNewPoints(const QList<QPoint> &);
  void setGeoCoordinates(const QList<QGeoCoordinate> &geoCoordinates);
  QGeoRectangle getGeoBounds() const;
  QGeoPath getGeoPath() const;
  bool hasGeoBounds() const;

  virtual void undoredo(QVariant=0);

  void read (const QJsonObject &json);
  void write (QJsonObject &json);


signals:
  void canGenerate(bool);
  void sceneRectChanged();
  void newUndoable(RGGraphicsObjectUndo *,QVariant);

public slots:
  void startPlayback(bool);
  void clearPath(bool clearGeoPath = false);
  void changePen(const QPen & pen);
  void activateTotalTime(bool);
  void activateSmoothPath(bool);
  void setRouteTime(int);
  void handleVehicleChange();
  void changePath(const QList<QPoint> &, bool);

protected slots:
  void handleMapLoaded(const QPixmap &map);

private:
  void updateVehicle();
  void processMapUpdate();

protected:
  RGMap *mMap;
  RGVehicleList *mVehicleList;

  //The generated/interpolated path, from the mEditPath
  RGPath  *mPath;

  //The actual points, edited by the user (or translated from the geopoints in mGeoPath)
  RGEditPath * mEditPath;
  QGeoPath mGeoPath;
  int mWidth, mHeight;
  bool mIconlessBeginEndFrames;
  bool mPlayback;
  bool mEditMode;
};

#endif // RGROUTE_H
