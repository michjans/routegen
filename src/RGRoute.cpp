#include "RGRoute.h"

#include <QDebug>

RGRoute::RGRoute(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mBoundingRect(QRectF()),
    mIconlessBeginEndFrames(false),
    mShowVehicle(false)
{
  mPath=new RGPath(this);
  mEditPath=new RGEditPath(this);
  mEditPath->setVisible(false);
  QObject::connect(mEditPath,SIGNAL(newPointList(QList<QPoint>)),this,SLOT(on_pathChanged(QList<QPoint>)));

  mRouteUi = new RGRouteUi();
  QObject::connect(mRouteUi,SIGNAL(penChanged(const QPen &)),this,SLOT(on_penChanged(const QPen &)));
  QObject::connect(mRouteUi,SIGNAL(smoothPathChecked(bool)),this,SLOT(on_smoothPathChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(totalTimeChecked(bool)),this,SLOT(on_totalTimeChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(routeTimeChanged(int)),this,SLOT(on_routeTimeChanged(int)));
  QObject::connect(mRouteUi,SIGNAL(vehicleChanged()),this,SLOT(on_vehicleChanged()));
  QObject::connect(this,SIGNAL(playback(bool)),mRouteUi,SLOT(on_playback(bool)));

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
  mRouteUi->setVehicleList(mVehicleList);

  //set initial by sending signals
  mRouteUi->init();
  }

QRectF RGRoute::boundingRect() const
{
     return mBoundingRect;
 }

void RGRoute::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  //painter->setBrush(Qt::black);
  //painter->drawRect(-10,-10,20,20);
}

QWidget *RGRoute::widgetSettings()
{
  return mRouteUi;
}

void RGRoute::setSmoothCoef(int dsmooth)
{
  mPath->setSmoothCoef(dsmooth);
}


void RGRoute::sceneRectChanged(const QRectF & rect)
{
  prepareGeometryChange();
  mBoundingRect=rect;
  mEditPath->sceneRectChanged(rect);
}

void RGRoute::on_penChanged(const QPen & pen)
{
  qDebug()<<"on_penChanged";
  mPath->setPen(pen);
}

void RGRoute::on_totalTimeChecked(bool checked)
{
  qDebug()<<"on_setTotalTimeChecked";
  if (checked)
    mPath->setPlayMode(1);
  else
    mPath->setPlayMode(0);
  //TODO update vehicle
}
void RGRoute::on_smoothPathChecked(bool checked)
{
  qDebug()<<"on_smoothPathChecked";
  mPath->setSmoothPath(checked);
  mVehicleList->getCurrentVehicle()->setPos(mPath->getEndPos());
  mVehicleList->getCurrentVehicle()->setRotation(mPath->getAngle());
}

void RGRoute::on_routeTimeChanged(int time)
{
  qDebug()<<"on_routeTimeChanged";
  mPath->setTotalTime(time);
}

void RGRoute::on_vehicleChanged()
{
  qDebug()<<"on_vehicleChanged, countFrames:"<<mPath->getEndPos();
  mVehicleList->getCurrentVehicle()->setParentItem(this);
  mVehicleList->getCurrentVehicle()->setPos(mPath->getEndPos());
  mVehicleList->getCurrentVehicle()->setRotation(mPath->getAngle());
  mVehicleList->getCurrentVehicle()->setVisible(mShowVehicle);
}

void RGRoute::on_pathChanged(QList<QPoint> pointlist)
{
  mPath->newPointList(pointlist);

  qDebug()<<"pointlist size"<<pointlist.size();
  if(pointlist.size()==2)
    emit canGenerate(true);
  if(pointlist.size()<2)
    emit canGenerate(false);
}

void RGRoute::setEditMode(bool checked)
{
  mPath->setCurrentFrame(mPath->countFrames());
  mEditPath->setVisible(checked);
  mShowVehicle=!checked;
  mVehicleList->getCurrentVehicle()->setPos(mPath->getEndPos());
  mVehicleList->getCurrentVehicle()->setRotation(mPath->getAngle());
  if(this->countFrames()<2) mShowVehicle=false;
  mVehicleList->getCurrentVehicle()->setVisible(mShowVehicle);
}

void RGRoute::clearPath()
{
  mEditPath->clear();
  mVehicleList->getCurrentVehicle()->setVisible(false);
}

int RGRoute::countFrames()
{
  return mPath->countFrames();
}

void RGRoute::setCurrentFrame(int frame)
{
  int time=mPath->setCurrentFrame(frame);
  //if(mIconlessBeginEndFrames==true && (frame==0 || frame==mPath->countFrames()))
    //mVehicle->setVisible(false);
  //else
    //mVehicle->setVisible(true);
 qDebug()<<"endPos "<<mPath->getEndPos();
 //mVehicleList->getVehicle(mVehicleList->getCurrentVehicleId())->setVisible();
  mVehicleList->getCurrentVehicle()->setPos(mPath->getEndPos());
  mVehicleList->getCurrentVehicle()->setRotation(mPath->getAngle());
  mVehicleList->getCurrentVehicle()->setTime(time);
}

void RGRoute::setIconlessBeginEndFrames(bool val)
{
  mIconlessBeginEndFrames=val;
}
