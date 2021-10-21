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

#include "RGVehicle.h"
#include <QDebug>
#include <QFileInfo>
#include <QImageReader>
#include <QPainter>

namespace
{
    //To prevent jerky vehicle movement, increase this number, so that the vehicle is only
    //rotated when the path's angle difference is greater than this number.
    const double gVehicleRotRange=20.0;
}

RGVehicle::RGVehicle(const QString &fileName,int size,bool mirror,int startAngle, bool acceptRotation,
                     QPointF originPoint, int frameDelay)
  :QGraphicsItem(),
    mFileName(fileName),
    mMirror(false),
    mStartAngle(startAngle),
    mSize(size),
    mRawSize(0),
    mFrameDelay(frameDelay),
    mCurrentPm(0),
    mOriginPoint(originPoint),
    mRotMirror(false),
    mAcceptRotation(acceptRotation)
{
  qDebug() << "RGVehicle::RGVehicle( " << fileName << ")";
  QImageReader qir(fileName);
  if (!qir.canRead()) {
    mRawPm.push_back(QPixmap());
  }

  if (qir.supportsAnimation()) {
    qDebug() << "   supports animation";
    //Store all frames
    int delay = qir.nextImageDelay();
    //If delay given in file, use delay
    if (delay != 0) mFrameDelay = delay;
    qDebug() << "   using delay = " << mFrameDelay;
    QImage im;
    do {
      im = qir.read();
      if (!im.isNull()) {
        qDebug() << "   adding animation frame";
        mRawSize=im.height();
        if(mRawSize<im.width())
          mRawSize=im.width();
        mRawPm.push_back(QPixmap::fromImage(im));
      }
    }
    while (!im.isNull());
  } else {
    //It's a single image
    qDebug() << "   adding single image";
    QImage im = qir.read();
    mRawSize=im.height();
    if(mRawSize<im.width())
      mRawSize=im.width();
    mRawPm.push_back(QPixmap::fromImage(im));
    mFrameDelay = 0; //Means, no animation
  }
  if(mOriginPoint.x()==-1.0 && mOriginPoint.y()==-1.0)
    mOriginPoint=QPointF(mRawSize/2,mRawSize/2);
  this->setSize(size);
  this->setStartAngle(startAngle);
  this->setMirror(mirror);
}

RGVehicle::~RGVehicle()
{
  //qDebug() << "RGVehicle::~RGVehicle( " << mFileName << ")";
}

QRectF RGVehicle::boundingRect() const
{
  return QRectF(0-mOriginPoint.x(),0-mOriginPoint.y(),mRawSize,mRawSize);
}

void RGVehicle::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->drawPixmap(0-mOriginPoint.x(),0-mOriginPoint.y(),mRawPm.at(mCurrentPm));
}

QPointF RGVehicle::getOrigin()
{
  return mOriginPoint;
}

void RGVehicle::setOrigin(QPointF point)
{
  this->prepareGeometryChange();
  mOriginPoint=point;
  update();
}

int RGVehicle::getSize()
{
  return mSize;
}

void RGVehicle::setSize(int size)
{
  if(size==0) //set to default
    size=mRawSize;
  mSize=size;
  if(mRawSize!=0){
    qreal scale = (double) size/mRawSize;
    this->setScale(scale);
  }
}

int RGVehicle::getRawSize()
{
  return mRawSize;
}

bool RGVehicle::getMirror()
{
  return mMirror;
}

void RGVehicle::setMirror(bool mirror)
{
  if(mMirror!=mirror)
  {
    this->setTransform(QTransform::fromScale(-1.0, 1.0), true);
  }

  mMirror=mirror;
}

int RGVehicle::getStartAngle()
{
  return mStartAngle;
}

void RGVehicle::setStartAngle(int startAngle)
{
  mStartAngle=startAngle;
  //Only now we want to rotate the vehicle as we like
  bool rememberedAcceptRotation = mAcceptRotation;
  mAcceptRotation = true;
  this->setRotation(0);//update rotation
  mAcceptRotation = rememberedAcceptRotation;
}

void RGVehicle::setRotation(qreal angle)
{
  if (!mAcceptRotation)
    return;

  while (angle < 0)
    angle += 360;
  while (angle > 360)
    angle -= 360;

  if (mRotMirror)
  {
      if(angle > 270.0 + gVehicleRotRange || angle < 90.0 - gVehicleRotRange)
      {
          mRotMirror=false;
          this->setTransform(QTransform::fromScale(-1.0, 1.0), true);
      }
  }
  else
  {
      if(angle < 270.0 - gVehicleRotRange && angle > 90.0 + gVehicleRotRange)
      {
          mRotMirror=true;
          this->setTransform(QTransform::fromScale(-1.0, 1.0), true);
      }
  }

  if (mRotMirror)
  {
      angle=180-angle;
  }

  if(mMirror) angle=-angle;
  QGraphicsItem::setRotation(angle+mStartAngle);
}

int RGVehicle::getDelay()
{
  return mFrameDelay;
}

void RGVehicle::setTime(int time)
{
  if (mFrameDelay!=0)
    mCurrentPm=time/mFrameDelay%mRawPm.size();
  update();
}

QPixmap RGVehicle::getPixmapAtSize(int size)
{
  QTransform transform;
  if(mMirror) transform.scale(-1,1);
  transform.rotate(mStartAngle);
  QPixmap pm=mRawPm.at(0).transformed(transform,Qt::SmoothTransformation);
  return pm.scaledToHeight(size);
}


QString RGVehicle::getName() const
{
  return QFileInfo(mFileName).baseName();
}

QString RGVehicle::getFileName() const
{
  return mFileName;
}
