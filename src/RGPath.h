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

#ifndef RGPATH_H
#define RGPATH_H

#include <QGraphicsObject>
#include <QPen>
#include <QtGui>

class RGPath : public QGraphicsObject
{
  Q_OBJECT
public:
  RGPath(QGraphicsItem *parent = 0);
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
  int countFrames();
  void setCurrentFrame(int);
  int getCurrentFrame();
  int getCurrentTime();
  QPointF getEndPos();
  float getAngle();

signals:

public slots:
  void newPointList(QList<QPoint>);
  void setDrawTime(int);
  void setSmoothPath(bool);
  void setSmoothCoef(int);
  void setTotalTime(int time);
  void setPlayMode(int playMode);
  void setFPS(int FPS);
  void setPen(const QPen & pen);


private:
  QPainterPath getPathAtStep(int step);
  QPainterPath getPathAtTime(int time);
  void createPath();
  void createSmoothPath();
  QPainterPath pathLineQuad(QPoint start,QPoint coef, QPoint end);
  QPainterPath pathLineCubic(QPoint start,QPoint coef1,QPoint coef2, QPoint end);
  QPoint getPointAtLength(QPoint start,QPoint end,int length);
  float getAngleAtTime(int time);
  float getAngleAtStep(int step);

private:
  QList<QPoint>     mRawPath;
  int               mTime;
  QPainterPath      mPath; //Processed final path
  QPainterPath      mPaintPath; //path that will be paint
  int               mTotalTime; //total time for interpolation
  int               mPlayMode; //set the mode for the video generation(0=stepbystep,1=TotalTimeSet,2=speedSet)
  int               mFPS;
  int               mDSmooth;
  bool              mSmoothPath;
  QPen              mPen;
  int               mCurrentFrame;
  QPointF            mEndPos;

};

#endif // RGPATH_H
