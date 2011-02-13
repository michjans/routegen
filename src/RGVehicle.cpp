/*
    Copyright (C) 2008-2011  Michiel Jansen
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

RGVehicle::RGVehicle(const QString &fileName,int size,bool mirror,int startAngle, QPointF originPoint,int frameDelay)
  :QGraphicsItem(),
  mFileName(fileName),
  mMirror(0),
  mStartAngle(startAngle),
  mSize(size),
  mRawSize(0),
  mFrameDelay(frameDelay),
  mOriginPoint(originPoint),
  mXmirror(false)
{
  qDebug() << "RGVehicle::RGVehicle( " << fileName << ")";
  QImageReader qir(fileName);
  if (!qir.canRead()) {
    mRawImages.push_back(QImage());
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
        mRawImages.push_back(im);
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
    mRawImages.push_back(im);
    mFrameDelay = 0; //Means, no animation
  }
  if(mOriginPoint.x()==-1 && mOriginPoint.y()==-1)
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
  //painter->setBrush(Qt::black);
  if(mRawImages.size()>=1){
      painter->drawPixmap(0-mOriginPoint.x(),0-mOriginPoint.y(),QPixmap::fromImage(mRawImages.at(0)));//getPixmap(0));
      //painter->setBrush(QBrush(Qt::black));
      //painter->drawEllipse(QPointF(0,0),5,5);
    }
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
    qDebug()<<"scale :"<< scale;
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
  //qDebug()<<"pre mirrorrect :"<<this->mapToScene(this->boundingRect());
  if(mMirror!=mirror)
    this->scale(-1,1);
  //qDebug()<<"post mirrorrect :"<<this->mapToScene(this->boundingRect());
  mMirror=mirror;
}

int RGVehicle::getStartAngle()
{
  return mStartAngle;
}

void RGVehicle::setStartAngle(int startAngle)
{
  mStartAngle=startAngle;
  this->setRotation(0);//update rotation
}

void RGVehicle::setRotation(qreal angle)
{
  qDebug()<<"angle"<<angle;
  if(angle<270 && angle>90 && mXmirror==false){
    mXmirror=true;
    this->scale(1,-1);
  }
  if((angle>270 || angle<90) && mXmirror==true){
    mXmirror=false;
    this->scale(1,-1);
  }
  QGraphicsItem::setRotation(angle+mStartAngle);
}

int RGVehicle::getDelay()
{
  return mFrameDelay;
}

/*QPixmap RGVehicle::getPixmap(int time)
{
  return getPixmapAtAngle(0,time);

}

QPixmap RGVehicle::getPixmapAtSize(int size)
{
  QPixmap pm = QPixmap(40,40);
  pm.fill(Qt::transparent);
  if (mImages.size() > 0){
    int lastSize=mSize;
    createImages(size,mStartAngle,mMirror);
    pm = QPixmap::fromImage(mImages[0]);
    createImages(lastSize,mStartAngle,mMirror);
  }
  return pm;
}

QPixmap RGVehicle::getPixmapAtAngle(int degrees, int time)
{
  if (mImages.size() == 0)
    return QPixmap();
  int angle = (int) degrees%360;
  int numImage=0;
  if (time!=0 && mFrameDelay!=0)
    numImage=time/mFrameDelay%mImages.size();
  QImage im = QImage(mImages[numImage]);
  if (angle>90 && angle <270){
    im=mirrorImage(im);
    angle +=180;
  }
  return QPixmap::fromImage(rotateImage(im,angle));
}*/

QString RGVehicle::getName()
{
  return QFileInfo(mFileName).baseName();
}

/*QImage RGVehicle::rotateImage(QImage &image, int degrees)
{
  QMatrix matrix;
  matrix.rotate((-1)*(qreal) degrees);
  return image.transformed(matrix, Qt::SmoothTransformation);
}

QImage RGVehicle::scaleImage(QImage &image, int size)
{
  mSize=size;
  if(image.width() > image.height()){
    if (size != image.width())
      return image.scaledToWidth(size,Qt::SmoothTransformation);
  }
  else {
    if (size != image.height())
      return image.scaledToHeight(size,Qt::SmoothTransformation);
  }
  return QImage();
}

QImage RGVehicle::mirrorImage(QImage &image)
{
  return image.mirrored(true, false);
}

void RGVehicle::createImages(int size, int angle, bool mirror)
{
  mImages.clear();
  if (mRawSize==0)
    return;
  for (int i=0;i<(int)mRawImages.size();++i){
    QImage im;
    im=mRawImages.at(i);
    if (size!=mRawSize)
      im=scaleImage(im, size);
    if (angle!=0)
      im=rotateImage(im, angle);
    if (mirror==true)
      im=mirrorImage(im);
    mImages.push_back(im);
    mMirror=mirror;
    mStartAngle=angle;
    mSize=size;
  }
}
*/
