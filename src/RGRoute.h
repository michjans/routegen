/*
    Copyright (C) 2009  Michiel Jansen

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

#ifndef RG_ROUTE_H
#define RG_ROUTE_H


#include <QList>
#include <QPoint>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QColor>

#include "RGVehicle.h"

class RGRoute
{
public:
  RGRoute(QList<QPoint> listpoint);

  ~RGRoute();

  void addPoint(QPoint newPoint);
  void clear();
  void drawPath(QPainter &painter);
  void drawPathAt(int frame,QPainter &painter);
  int getNumberFrame();
  int userPointCount();
  void setTotalTime(int time);
  void setPlayMode(int playMode);
  void setFPS(int FPS);
  QPen getPen();
  void setPen(const QColor &color,int size,Qt::PenStyle style);
  void setVehicle(const RGVehicle &vehicle);
  void setIconlessBeginEndFrames(bool);
  void removefromPoint(int);

private:
  QPainterPath createPath(QList<QPoint> RawPath);
  QPainterPath getPathAtStep(int step);
  QPainterPath getPathAtTime(int time);
  float getAngleAt(int frame);
  float getAngleAtTime(int time);
  float getAngleAtStep(int step);

private:
  QList<QPoint>     mRawRoute;
  QPainterPath      mPath; //Processed final path
  int               mR; //Radius to use for circle segments
  int               mTotalTime; //total time for interpolation
  int               mPlayMode; //set the mode for the video generation(0=stepbystep,1=TotalTimeSet,2=speedSet)
  int               mFPS;
  //Pen
  QColor            mPenColor;
  Qt::PenStyle      mPenStyle;
  int               mPenSize;
  //The icon that's drawn at the end of the path (e.g. train, bike)
  RGVehicle         mVehicle;
  bool              mIconlessBeginEndFrames;
  QList<int>        mUndoBuffer;
};


#endif //RG_ROUTE_H
