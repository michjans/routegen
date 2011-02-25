#include "RGPath.h"
#include <math.h>

const float Pi = 3.14159265f;

RGPath::RGPath(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mRawPath(QList<QPoint>()),
    mTime(0),
    mPath(QPainterPath()),
    mTotalTime(0),
    mPlayMode(0),
    mFPS(25),
    mDSmooth(10),
    mSmoothPath(false),
    mPen(QPen()),
    mCurrentFrame(0),
    mEndPos(QPoint())
{

}

QRectF RGPath::boundingRect() const
{
    return mPath.boundingRect();
 }

void RGPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setPen(mPen);
  //painter->drawPath(mPath);
  qDebug()<<"paint path at Current Frame "<<mCurrentFrame;
  if (mCurrentFrame<0){
    painter->drawPath(mPath);
    mEndPos=mPath.pointAtPercent(1);
    return;
  }
  QPainterPath tmpPath;
  int time=mCurrentFrame*(1.0 / (double) mFPS) * 1000;
  if (mPlayMode==0) tmpPath=getPathAtStep(mCurrentFrame);
  if (mPlayMode==1) tmpPath= getPathAtTime(time);
  painter->drawPath(tmpPath);
  mEndPos=tmpPath.pointAtPercent(1);
}

QPainterPath RGPath::getPathAtStep(int step) //return Path at step (begin at 0)
{
  if (mPath.elementCount()==0)
    return QPainterPath();
  if (step>=mPath.elementCount())
    return mPath;
  QPainterPath tmpPath;
  tmpPath.moveTo(mPath.elementAt(0).x,mPath.elementAt(0).y);
  for (int i=1;i<=step;++i){
    tmpPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
  }
  return tmpPath;
}

QPainterPath RGPath::getPathAtTime(int time) //return Path at time in ms
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
  for (int i=1;i<mPath.elementCount();++i){
    testPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
    if(testPath.length()>=newlength){
      //the i element contains the point at time
      goodPath.lineTo(newpointf);
      break;
    }
    //if no break, the whole segment is added to the goodPath
    goodPath.lineTo(mPath.elementAt(i).x,mPath.elementAt(i).y);
  }
  return goodPath;
}



int RGPath::countFrames()
{
  //stepbystep
  if (mPlayMode==0) return mPath.elementCount();
  //Interpolation TotalTime set
  if (mPlayMode==1 && mPath.elementCount()>1) return mFPS * mTotalTime+1;
  return 0;
}

int RGPath::setCurrentFrame(int frame)
{
  mCurrentFrame=frame;
  update();
  return mCurrentFrame*(1.0 / (double) mFPS) * 1000;
}

QPointF RGPath::getEndPos()
{
  return mEndPos;
}

float RGPath::getAngle()
{
  qreal percent=0;
  if (mPlayMode==0){
    qreal length=0;
    //TODO angle for step 0 !
    for(int i=1;i<mCurrentFrame;i++){
      length+= QLineF(mPath.elementAt(i).x,mPath.elementAt(i).y,mPath.elementAt(i-1).x,mPath.elementAt(i-1).y).length();
    }
    percent = (double) length / ((double) mPath.length());//mPath.length should never be NULL
  }
  if (mPlayMode==1)
    percent = (double) (mCurrentFrame*(1.0 / (double) mFPS)) / ((double) mTotalTime);//mTotalTime should never be null;
  if(percent>1)
    percent=1;
  qreal angle=mPath.angleAtPercent(percent);
  qDebug()<<"pre angle"<<angle<<"angle path return"<<(360-angle);
  return (360-angle);
}

void RGPath::newPointList(QList<QPoint> pointList)
{
  qDebug()<<"signal newPointList received";

  if(pointList.size()<0)
    return;
  mRawPath=pointList;
  createPath();
  mCurrentFrame=this->countFrames();
  mEndPos=QPoint(mRawPath.at(0));
  if (pointList.size()<2)
    mEndPos=QPoint(-200,-200);
}

void RGPath::setDrawTime(int time)
{
  mTime=time;
}

void RGPath::setSmoothPath(bool smooth)
{
  mSmoothPath=smooth;
  createPath();
}

void RGPath::setSmoothCoef(int dSmooth)
{
  mDSmooth=dSmooth;
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
  prepareGeometryChange();
  //create path from data :
  mPath = QPainterPath();
  if (mRawPath.count()<=1){
    update();
    return;
  }
  if(mSmoothPath){
    createSmoothPath();
  }
  else {
    mPath.moveTo(mRawPath.at(0));
    for (int i=1;i<mRawPath.count();++i){
      mPath.lineTo(mRawPath.at(i));
    }
  }
  update();
}

void RGPath::createSmoothPath()
{
  mPath=QPainterPath();
  if(mRawPath.size()<=1)
    return ;
  QPainterPath newPath(mRawPath.at(0));
  QPoint A=mRawPath.at(0),B=mRawPath.at(1),p1,c1,p2,c2,sc2;
  int dist1=0;
  bool haveP2=false;
  double dAB=sqrt(pow(double((B-A).x()), 2) + pow(double((B-A).y()), 2));
  c1=B;
  if(dAB > mDSmooth) {
    p1=getPointAtLength(B,A,mDSmooth);
  }
  else{
    p1=getPointAtLength(A,B,1);//line one point before connect(path)
  }
  newPath.lineTo(p1);

  for (int i=1;i<mRawPath.count()-1;++i)
  {
    A=mRawPath.at(i);
    B=mRawPath.at(i+1);
    dAB=sqrt(pow(double((B-A).x()), 2) + pow(double((B-A).y()), 2));
    if (haveP2) {
      dist1+= (int) dAB;
      if (dist1>=mDSmooth){
        sc2=getPointAtLength(B,A,dist1-mDSmooth);
        c2=2*p2-sc2;
        newPath.connectPath(pathLineCubic(p1,(c1-p1)/4+p1,(c2-p2)/4+p2,p2));
        p1=p2;
        c1=2*p1-c2;
        if (dAB>mDSmooth){
          haveP2=false;
          p2=getPointAtLength(A,B,mDSmooth);
          c2=A;
          newPath.connectPath(pathLineCubic(p1,(c1-p1)/4+p1,(c2-p2)/4+p2,p2));
          dist1=dAB-mDSmooth;
          p1=p2;
          c1=2*p1-c2;
          if(dAB>2*mDSmooth){
            p1=getPointAtLength(B,A,mDSmooth);
            newPath.lineTo(p1);
            c1=B;
            dist1=0;
          }
        }
        else {
          p2=c1;
          dist1=sqrt(pow(double((B-p2).x()), 2) + pow(double((B-p2).y()), 2));
        }
      }
    }
    else {
      if(dAB>=2*mDSmooth) {
        c1=A;
        p2=getPointAtLength(A,B,mDSmooth);
        newPath.connectPath(pathLineQuad(p1,c1,p2));
        //newPath.addRect(c1.x(),c1.y(),2,2);
        p1=getPointAtLength(B,A,mDSmooth);
        newPath.lineTo(p1);
        c1=B;
      }
      else {
        dist1+= (int) dAB;
        if (dist1>=mDSmooth){
          p2=getPointAtLength(B,A,dist1-mDSmooth);
          haveP2=true;
          dist1=sqrt(pow(double((B-p2).x()), 2) + pow(double((B-p2).y()), 2));
        }
      }
    }
  }
  if(dAB>=mDSmooth)
    newPath.lineTo(B);
  else
    newPath.connectPath(pathLineCubic(p1,(c1-p1)/4+p1,(A-B)/4+B,B));
  mPath=newPath;
}

QPoint RGPath::getPointAtLength(QPoint start,QPoint end,int length)
{
  QPoint dv = end-start;
  double totlength=sqrt(pow(double((end-start).x()), 2) + pow(double((end-start).y()), 2));
  return QPoint(start+length/totlength*dv);
}

QPainterPath RGPath::pathLineQuad(QPoint start,QPoint coef, QPoint end)
{
  QPainterPath curvePath(start),linearPath(start);
  curvePath.quadTo(coef,end);
  int nbPoints = (int) curvePath.length()/4;
  for (int i=0;i<nbPoints;i++){
    linearPath.lineTo(curvePath.pointAtPercent(i*1.0/nbPoints));
  }
  linearPath.lineTo(end);
  //qDebug()<<"d of quad:"<<curvePath.length();
  return linearPath;
}

QPainterPath RGPath::pathLineCubic(QPoint start,QPoint coef1,QPoint coef2, QPoint end)
{
  QPainterPath curvePath(start),linearPath(start);
  curvePath.cubicTo(coef1,coef2,end);
  int nbPoints = (int) curvePath.length()/4;
  for (int i=0;i<nbPoints;i++){
    linearPath.lineTo(curvePath.pointAtPercent(i*1.0/nbPoints));
  }
  linearPath.lineTo(end);
  //qDebug()<<"d of quad:"<<curvePath.length();
  return linearPath;
}
