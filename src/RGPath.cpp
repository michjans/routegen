#include "RGPath.h"

RGPath::RGPath(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    mRawPath(QList<QPoint>()),
    mTime(0),
    mPath(QPainterPath()),
    mTotalTime(0),
    mPlayMode(0),
    mFPS(25),
    mIconlessBeginEndFrames(false),
    mDSmooth(10),
    mSmoothPath(false),
    mPen(QPen())
{

}

QRectF RGPath::boundingRect() const
{
     return QRectF(0,0,300,300);
 }

void RGPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setPen(mPen);
  painter->drawPath(mPath);
}

void RGPath::newPointList(QList<QPoint> pointList)
{
  qDebug()<<"signal newPointList received";

  if(pointList.size()<=0)
    return;
  mRawPath=pointList;
  createPath();
  update();
}

void RGPath::setDrawTime(int time)
{
  mTime=time;
}

void RGPath::setIconlessBeginEndFrames(bool val)
{
  mIconlessBeginEndFrames=val;
}

void RGPath::setSmoothPath(bool smooth)
{
  mSmoothPath=smooth;
  createPath();
}

void RGPath::setSmoothCoef(int dsmooth)
{
  mDSmooth=dsmooth;
  createPath();
}

void RGPath::setTotalTime(int time)//set mTotaltime in ms
{
  if (time!=0)
    mTotalTime=time;
  //what to do if time=0 ?
}

void RGPath::setPlayMode(int mode)
{
  if (mode>=0 && mode<3)
    mPlayMode=mode;
  else
    mPlayMode=0;
  createPath();
}

void RGPath::setFPS(int FPS)
{
  mFPS=FPS;
}

void RGPath::setPen(const QPen & pen)
{
  mPen=pen;
  update();
}

void RGPath::createPath()
{
  //create path from data :
  mPath = QPainterPath();
  if (mRawPath.count()<=1)
    return;
  if(mSmoothPath){
    //mPath = RGSmoothRoute::SmoothRoute(mRawPath,mDSmooth);
  }
  else {
    mPath.moveTo(mRawPath.at(0));
    for (int i=1;i<mRawPath.count();++i){
      mPath.lineTo(mRawPath.at(i));
    }
  }
}
