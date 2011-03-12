#include "RGRoute.h"

#include <QDebug>

RGRoute::RGRoute(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mIconlessBeginEndFrames(false),
    mPlayback(false),
    mEditMode(false)
{
  mPath=new RGPath(this);
  mEditPath=new RGEditPath(this);
  mEditPath->setVisible(false);
  QObject::connect(mEditPath,SIGNAL(newPointList(QList<QPoint>)),this,SLOT(on_pathChanged(QList<QPoint>)));
  QObject::connect(this,SIGNAL(sceneRectChanged()),mEditPath,SLOT(on_sceneRectChanged()));

  mRouteUi = new RGRouteUi();
  QObject::connect(mRouteUi,SIGNAL(penChanged(const QPen &)),this,SLOT(on_penChanged(const QPen &)));
  QObject::connect(mRouteUi,SIGNAL(smoothPathChecked(bool)),this,SLOT(on_smoothPathChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(totalTimeChecked(bool)),this,SLOT(on_totalTimeChecked(bool)));
  QObject::connect(mRouteUi,SIGNAL(routeTimeChanged(int)),this,SLOT(on_routeTimeChanged(int)));
  QObject::connect(mRouteUi,SIGNAL(vehicleChanged()),this,SLOT(on_vehicleChanged()));

  //create and set up vehicleList
  mVehicleList = new RGVehicleList();
  mRouteUi->setVehicleList(mVehicleList);

  //set initial by sending signals
  mRouteUi->init();
  }

QRectF RGRoute::boundingRect() const
{
     return QRectF();//mBoundingRect;
 }

void RGRoute::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  Q_UNUSED(painter);
}

QWidget *RGRoute::widgetSettings()
{
  return mRouteUi;
}

void RGRoute::setSmoothCoef(int dsmooth)
{
  mPath->setSmoothCoef(dsmooth);
  setCurrentFrame(mPath->countFrames()-1);
}

void RGRoute::on_playbackChanged(bool play)
{
  mPlayback=play;
  mRouteUi->on_playbackChanged(mPlayback);
}

void RGRoute::on_penChanged(const QPen & pen)
{
  mPath->setPen(pen);
}

void RGRoute::on_totalTimeChecked(bool checked)
{
  if (checked)
    mPath->setPlayMode(1);
  else
    mPath->setPlayMode(0);
}

void RGRoute::on_smoothPathChecked(bool checked)
{
  mPath->setSmoothPath(checked);
  //if playback is on, update to the current frame else to the last :
  if(mPlayback)
    setCurrentFrame(mPath->getCurrentFrame());
  else
    setCurrentFrame(mPath->countFrames()-1);
}

void RGRoute::on_routeTimeChanged(int time)
{
  mPath->setTotalTime(time);
}

void RGRoute::on_vehicleChanged()
{
  mVehicleList->getCurrentVehicle()->setParentItem(this);
  updateVehicle();
}

void RGRoute::on_pathChanged(QList<QPoint> pointlist)
{
  mPath->newPointList(pointlist);

  if(pointlist.size()==2)
    emit canGenerate(true);
  if(pointlist.size()<2)
    emit canGenerate(false);
}

void RGRoute::setEditMode(bool checked)
{
  mEditMode=checked;
  mEditPath->setVisible(checked);
  setCurrentFrame(mPath->countFrames()-1);
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
  if(mEditMode){
    mVehicleList->getCurrentVehicle()->setVisible(false);
    return;
  }
  int frame=mPath->getCurrentFrame();
  if(frame<2){
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
