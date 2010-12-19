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


#include "RGRoute.h"
#include "RGSmoothRoute.h"
#include <math.h>
#include <QDebug>


const float Pi = 3.14159265f;


RGRoute::RGRoute(QList<QPoint> listPoint)
: mRawRoute(listPoint),
mR(0),
mTotalTime(10000),
mPlayMode(0),
mFPS(25),
mPenColor(Qt::blue),
mPenStyle(Qt::SolidLine),
mPenSize(5),
mVehicle(),
mIconlessBeginEndFrames(false)
{
  mPath = createPath(listPoint);
  mUndoBuffer.append(0);
}

RGRoute::~RGRoute()
{

}

void RGRoute::addPoint(QPoint newPoint)
{
    mUndoBuffer.append(mRawRoute.size());
    mRawRoute.append(newPoint);
    mPath = createPath(mRawRoute);
}

void RGRoute::clear()
{
    mRawRoute.clear();
    mPath=createPath(mRawRoute);
}

QPainterPath RGRoute::createPath(QList<QPoint> RawRoute)
{
    QPainterPath tmpPath,goodPath;
    //create path from data :
    if (RawRoute.count()<=1)
        return tmpPath;
    if(mSmoothPath){
      tmpPath = RGSmoothRoute::SmoothRoute(RawRoute,mDSmooth);
    }
    else {
        tmpPath.moveTo(RawRoute.at(0));
        for (int i=1;i<RawRoute.count();++i)
        {
          tmpPath.lineTo(RawRoute.at(i));
        }
    }

    //truncate every 20 if mPlayMode>=1
    /*if(mPlayMode>=1){
      QPoint A=QPoint(tmpPath.elementAt(0).x,tmpPath.elementAt(0).y),B;
      goodPath.moveTo(A);
      int dAB=0;
      for (int i=1;i<tmpPath.elementCount();++i)
      {
        B=QPoint(tmpPath.elementAt(i).x,tmpPath.elementAt(i).y);
        dAB=sqrt(pow((B-A).x(), 2) + pow((B-A).y(), 2));
        while (dAB>20){
          A=A+20/(double) dAB*(B-A);
          goodPath.lineTo(A);
          dAB-=20;
        }
        goodPath.lineTo(B);
        A=B;
      }
    }
    else*/
      goodPath=tmpPath;
    return goodPath;
}

void RGRoute::drawPath(QPainter &painter)
{
    painter.drawPath(mPath);
}

void RGRoute::drawPathAt(int frame,QPainter &painter)
{
    QPainterPath tmpPath;
    if (mPlayMode==0) tmpPath=getPathAtStep(frame);
    if (mPlayMode==1) tmpPath= getPathAtTime(frame*(1.0 / (double) mFPS) * 1000);
    painter.drawPath(tmpPath);
    if (mVehicle.getSize()==0 || tmpPath.elementCount()==0)
        return;
    if (mIconlessBeginEndFrames && (frame==0 || frame>=(getNumberFrame()-1)))
        return;
    //TODO: get animated gif working
    float angle;
    angle=getAngleAt(frame);
    QPixmap vehim = mVehicle.getPixmap(angle);
    //Draw vehicle with center on current point
    int px = tmpPath.elementAt(tmpPath.elementCount()-1).x - vehim.size().width() / 2;
    int py = tmpPath.elementAt(tmpPath.elementCount()-1).y - vehim.size().height() / 2;
    painter.drawPixmap(px, py, vehim);
}

QPainterPath RGRoute::getPathAtStep(int step) //return Path at step (begin at 0)
{
    if (mPath.elementCount()==0)
        return QPainterPath();
    if (step>=mPath.elementCount())
        return mPath;
    QPainterPath tmpPath;
    tmpPath.moveTo(mPath.elementAt(0).x,mPath.elementAt(0).y);
    for (int i=1;i<=step;++i)
    {
        tmpPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
    }
    return tmpPath;
}

QPainterPath RGRoute::getPathAtTime(int time) //return Path at time in ms
{

   QPointF newpointf;
    qreal percent = (double) time / ((double) mTotalTime*1000);//mTotalTime should never be null
    if(percent>=1)
        return mPath;
    if (mPath.elementCount()==0)
        return QPainterPath();
    newpointf = mPath.pointAtPercent(percent);
    qreal newlength = percent*mPath.length();
    //Now locate on which segment the length is obtained:
    QPainterPath goodPath,testPath;
    testPath.moveTo(mPath.elementAt(0).x,mPath.elementAt(0).y);
    goodPath.moveTo(mPath.elementAt(0).x,mPath.elementAt(0).y);
    for (int i=1;i<mPath.elementCount();++i)
    {
        testPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
        if(testPath.length()>=newlength)
        {
            //the i element contains the point at time
            goodPath.lineTo(newpointf);
            break;
        }
        //if no break, the whole segment is added to the goodPath
        goodPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
    }
    return goodPath;
}

float RGRoute::getAngleAt(int frame)
{
    if (mPlayMode==0) return getAngleAtStep(frame);
    if (mPlayMode==1) return getAngleAtTime(frame*(1.0 / (double) mFPS) * 1000);
    return 0;
}

float RGRoute::getAngleAtTime(int time)
{
    qreal percent = (double) time / ((double) mTotalTime*1000);//mTotalTime should never be null
    qreal angle=mPath.angleAtPercent(percent);
    return angle;

}

float RGRoute::getAngleAtStep(int step)
{
    //return angle of the previous segment
    int realstep=step;
    if (step>=mPath.elementCount())
        realstep=mPath.elementCount()-1;
    if(step==0)//return angle of the first element
        realstep=1;
    float dx,dy;
    dy=mPath.elementAt(realstep).y- mPath.elementAt(realstep-1).y;
    dx=mPath.elementAt(realstep).x- mPath.elementAt(realstep-1).x;
    if(dx==0 && dy>0)
        return (float) 270;
    if(dx==0 && dy<0)
        return (float) 90;
    float angle = ((atanf(dy /dx) / Pi) * 180.0);
    if(dx>0 && dy>=0)
        angle=-1*angle;
    else if (dx<0 && dy>=0)
        angle=180-angle;
    else if (dx<0 && dy<=0)
        angle=180-angle;
    else if (dx>0 && dy<=0)
        angle=-1*angle;
    else
        angle = 0;
    return angle;
}

void RGRoute::setTotalTime(int time)//set mTotaltime in ms
{
   if (time!=0)
       mTotalTime=time;
   //what to do if time=0 ?
}

void RGRoute::setPlayMode(int mode)
{
    if (mode>=0 && mode<3)
        mPlayMode=mode;
    else
        mPlayMode=0;
    mPath = createPath(mRawRoute);
}

void RGRoute::setFPS(int FPS)
{
    mFPS=FPS;
}

int RGRoute::getNumberFrame()
{
    //stepbystep
    if (mPlayMode==0) return mPath.elementCount();
    //Interpolation TotalTime set
    if (mPlayMode==1) return mFPS * mTotalTime+1;
    //Interpolation Speed set
    //if (mPlayMode==2) return
    return 0;
}

int RGRoute::userPointCount()
{
    return mRawRoute.size();
}


QPen RGRoute::getPen()
{
    return QPen(QBrush(mPenColor, Qt::SolidPattern), mPenSize, mPenStyle, Qt::FlatCap, Qt::RoundJoin);
}

void RGRoute::setPen(const QColor &color,int size,Qt::PenStyle style)
{
    mPenColor=color;
    mPenStyle=style;
    mPenSize=size;
}

void RGRoute::setVehicle(const RGVehicle &vehicle)
{
    mVehicle = vehicle;
}

void RGRoute::setIconlessBeginEndFrames(bool val)
{
    mIconlessBeginEndFrames=val;
}

void RGRoute::setSmoothPath(bool smooth)
{
  mSmoothPath=smooth;
  mPath=createPath(mRawRoute);
}

void RGRoute::setSmoothCoef(int dsmooth)
{
  mDSmooth=dsmooth;
  mPath=createPath(mRawRoute);
}

void RGRoute::removefromPoint(int idx)
{
    while (idx < mRawRoute.size())
        mRawRoute.removeLast();
    mPath=createPath(mRawRoute);
}

