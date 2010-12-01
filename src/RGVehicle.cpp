/*
    Copyright (C) 2008  Michiel Jansen

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

#include <QtGui>

#include <math.h>
#include <QtGui>

#include <QDebug>

#include "RGVehicle.h"

RGVehicle::RGVehicle(const QString &fileName, bool mirror, int startAngle, int defaultDelay)
  : mFrameDelay(defaultDelay),
    mStepAngle(12),     //Minimum angle difference, before rotating the image
    mMinDistance(10),   //Minimum distance between points, before calculating new angle
    mForceCounter(20),  //After this number always force a new pixmap (this number should
                        //be greater than mMinDistance otherwise it goes wrong in getPixmap)
    mCachedAngle(0),
    mCachedMirrorRequired(false),
    mCachedTime(0),
    mCacheInitialized(false),
    mCachedCounter(0)
{
	qDebug() << "RGVehicle::RGVehicle( " << fileName << ")";
  QImageReader qir(fileName);
  if (!qir.canRead()) {
    mImages.push_back(QImage());
    return;
  }

  if (qir.supportsAnimation()) {
    qDebug() << "   supports animation";
    //Store all frames
    mFrameDelay = qir.nextImageDelay();
    //If delay not given in file, use passed delay
    if (mFrameDelay == 0) mFrameDelay = defaultDelay;
    qDebug() << "   using delay = " << mFrameDelay;
    QImage im;
    do {
      im = qir.read();
      if (!im.isNull()) {
        qDebug() << "   adding animation frame";
        if (mirror)            im = im.mirrored(true, false);
        if (startAngle != 0.0) im = rotateImage(im, startAngle);
        mImages.push_back(im);
      }
    }
    while (!im.isNull());
  } else {
    //It's a single image
    qDebug() << "   adding single image";
    QImage im = qir.read();
    if (mirror)            im = im.mirrored(true, false);
    if (startAngle != 0.0) im = rotateImage(im, startAngle);
    mImages.push_back(im);
    mFrameDelay = INT_MAX; //Means, no animation
  }
}

RGVehicle::~RGVehicle()
{
    qDebug() << "RGVehicle::~RGVehicle";
    qDebug() << "  mCachedPm = " << mCachedPm.isNull();
}


void RGVehicle::setStepAngle(int stepAngle)
{
  mStepAngle = stepAngle;
}

void RGVehicle::setMinDistance(int distance)
{
  mMinDistance = distance;
}

void
RGVehicle::setForceCounter(int forceCtr)
{
  mForceCounter = forceCtr;
}

void RGVehicle::setStartPoint(const QPoint &from)
{
  mCachedPoint = from;
  mCachedAngle = 0;
  mCachedMirrorRequired = false;
  mCachedTime = 0;
  mCacheInitialized = false;
  mCachedCounter = 0;
  qDebug() << "RGVehicle::setStartPoint(" << from << ")";
}


QPixmap RGVehicle::getPixmap(int time, const QPoint &to)
{
  qDebug() << "RGVehicle::getPixmap( time = " << time << ", to = " << to << ")";
  qDebug() << "  mCachedAngle      = " << mCachedAngle;
  qDebug() << "  mCachedPoint      = " << mCachedPoint;
  qDebug() << "  mCacheInitialized = " << mCacheInitialized;
  qDebug() << "  mCachedTime       = " << mCachedTime;
  qDebug() << "  mStepAngle        = " << mStepAngle;
  qDebug() << "  mFrameDelay       = " << mFrameDelay;
  
  int angle = mCachedAngle;

  QPoint dp = to - mCachedPoint;
  int distance = (int) sqrt((float)(dp.x() * dp.x()) + (float)(dp.y() * dp.y()));
  qDebug() << "  distance = " << distance;
  
  //Only calc new angle distance is large enough, we need a new frame from an animation, or cache still uninitialized
  if (!mCacheInitialized || distance > mMinDistance) {
    qDebug() << "    distance > " << mMinDistance;
    //First prevent division by zero
    if (dp.x() == 0) {
      qDebug() << "    dp.x() == 0, so forcing angle to +-90...";
      if (dp.y() < 0) angle = -90.0;
      else if (dp.y() > 0)  angle = 90.0;
      else if (dp.y() == 0) angle =  0.0;
    } else {
      angle = (int) ((atanf((float)dp.y() / (float)dp.x()) / 3.14) * 180.0);
    }
    qDebug() << "    angle = " << angle;
  }

  qDebug() << "abs(" << angle << " - " << mCachedAngle << ") = " << abs(abs(angle) - abs(mCachedAngle));

  //We only do a new rotation when the angle change is big enough or we reached the mForceCounter
  //(When the cached vehicle had the same mirrored-ness as the possible new vehicle, we should use
  // can just use the angles, otherwise we have to use the absolute values of the angles, because
  // then the angles are swapped! (dp.x() < 0, means we should mirror))
  int dangle;
  if (dp.x() < 0 == mCachedMirrorRequired)
    dangle = abs(angle - mCachedAngle);
  else
    dangle = abs(abs(angle) - abs(mCachedAngle));

  bool newRotationRequired = dangle > mStepAngle ||
                             mCachedCounter > mForceCounter;

  //Return cached pixmap when nothing to rotate and no new animation required
  if (mCacheInitialized && !newRotationRequired && time - mCachedTime < mFrameDelay) {
    qDebug() << "  returning cached image";
    mCachedCounter++;
    return mCachedPm;
  }

  //New rotation required, so we store the new cache values
  if (newRotationRequired) {
    mCachedAngle = angle;
    mCachedCounter = 0;
    mCachedPoint = to;
    mCachedMirrorRequired = dp.x() < 0;
  }

  qDebug() << "  calculating new image";
  QImage im;
  if (mFrameDelay == INT_MAX) {
    //No animation, just use one and only image
    im = mImages[0];
  } else {
    //Use image frame, depending on time (in ms)
    int idx = (time / mFrameDelay) % mImages.size();
    im = mImages[idx];
  }

  if (mCachedMirrorRequired) {
    qDebug() << "  mCachedMirrorRequired < 0, so mirror image...";
    //Mirror first
    im = im.mirrored(true, false);
  }
    
  im = rotateImage(im, mCachedAngle);
  
  //Cache pixmap
  mCachedTime = time;
  mCacheInitialized = true;
  mCachedPm = QPixmap::fromImage(im);
  
  qDebug() << "  returning new rotated image";  
  return mCachedPm;
}

QPixmap RGVehicle::getPixmap()
{
  if (mImages.size() > 0)
    return QPixmap::fromImage(mImages[0]);
  else
    return QPixmap();
}

QImage RGVehicle::rotateImage(QImage &image, int degrees)
{
  QMatrix matrix;
  matrix.rotate((qreal) degrees);
  return image.transformed(matrix, Qt::SmoothTransformation);
}


