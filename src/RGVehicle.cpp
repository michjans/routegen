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

RGVehicle::RGVehicle(const QString &fileName,int size,bool mirror,int startAngle,int frameDelay)
  :mFileName(fileName),
  mMirror(mirror),
  mStartAngle(startAngle),
  mSize(size),
  mRawSize(0),
  mFrameDelay(frameDelay)
{
  qDebug() << "RGVehicle::RGVehicle( " << fileName << ")";
  QImageReader qir(fileName);
  if (!qir.canRead()) {
    mRawImages.push_back(QImage());
  }

  if (qir.supportsAnimation()) {
    qDebug() << "   supports animation";
    //Store all frames
    mFrameDelay = qir.nextImageDelay();
    //If delay not given in file, use passed delay
    if (mFrameDelay == 0) mFrameDelay = frameDelay;
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
  if(size==0)
    size=mRawSize;
  createImages(size,startAngle,mirror);
}

RGVehicle::~RGVehicle()
{
  //qDebug() << "RGVehicle::~RGVehicle( " << mFileName << ")";
}

int RGVehicle::getSize()
{
  if (mSize==0)
    return mRawSize;
  return mSize;
}

void RGVehicle::setSize(int size)
{
  if(size==0)
    size=mRawSize;
  createImages(size,mStartAngle,mMirror);
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
  createImages(mSize,mStartAngle,mirror);
}

int RGVehicle::getStartAngle()
{
  return mStartAngle;
}

void RGVehicle::setStartAngle(int startAngle)
{
  createImages(mSize,startAngle,mMirror);
}

int RGVehicle::getDelay()
{
  return mFrameDelay;
}

QPixmap RGVehicle::getPixmap(int time)
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
}

QString RGVehicle::getName()
{
  return QFileInfo(mFileName).baseName();
}

QImage RGVehicle::rotateImage(QImage &image, int degrees)
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
