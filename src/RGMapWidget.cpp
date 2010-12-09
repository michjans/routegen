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

#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCheckBox>
#include <QDebug>
#include <math.h>

#include "RGMapWidget.h"
#include "RGRoute.h"
#include "RGSettings.h"

#define MIN_PATH_LENGTH 2

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

//Defined in main.cpp
extern const QString applicationName;


RGMapWidget::RGMapWidget(QWidget *parent)
:QWidget(parent),
 mVehicle(NULL),
 mRgr(NULL),
 mFPS(25),
 mInDrawMode(false),
 mInitPhase(true),
 mGenerateBeginEndFrames(false),
 mPlayTimer(NULL),
 mTimerCounter(0)
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  mFPS = RGSettings::getFps();
  if (mRgr)
      delete mRgr;
  QList<QPoint> test;
  mRgr = new RGRoute(test);
  mRgr->setFPS(mFPS);
  mRgr->setTotalTime(RGSettings::getRoutePlayTime());
  mRgr->setPlayMode((int) RGSettings::getInterpolationMode());
  if (mPlayTimer == NULL) {
    mPlayTimer = new QTimer(this);
    QObject::connect(mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerEvent()));
  }
}

void RGMapWidget::loadImage(const QPixmap &pm)
{
  mImage = pm;
  mInitPhase = false;
  update();
  updateGeometry();
  //adjustSize();
}

void RGMapWidget::setVehicle(const QString &fileName, bool mirror, int startAngle, int size)
{
    if (mVehicle != NULL) {
        delete mVehicle;
        mVehicle=NULL;
    }
  if (!fileName.isNull()) {
    mVehicle = new RGVehicle(fileName, mirror, startAngle, size);

    //User might have overridden advanced settings for the vehicle
    mVehicle->setForceCounter(RGSettings::getVehicleForceCounter());
    mVehicle->setMinDistance(RGSettings::getVehicleMinDistance());
    mVehicle->setStepAngle(RGSettings::getVehicleStepAngle());
  }
  update();
}
  

QPixmap RGMapWidget::getImage()
{
  QPixmap newPixMap(mImage);
  QPainter painter(&newPixMap);
  painter.drawPixmap(0, 0, mImage);
  drawPath(painter);

  return newPixMap;
}

QPixmap RGMapWidget::getVehiclePixmap()
{
  if (mVehicle != NULL) return mVehicle->getPixmap();
  else return QPixmap();
}

bool RGMapWidget::generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs)
{
  //Disable draw mode automatically
  if (mInDrawMode)
    endDrawMode();

  generatedBMPs.clear();
  //Only usefull when route has more than MIN_PATH_LENGTH points
  if (mRgr->stepCount() < MIN_PATH_LENGTH) {
    QMessageBox::warning (this, "Route too short", "Sorry, this route is too short to generate a valid route.");
    return false;
  }

  bool generationOK = true;
  emit busy(true);
  //QString text = QString("Number of frames: ") + QString::number(mRgr->stepCount());

  QProgressDialog progress("Generating files...", "Abort", 0, mRgr->getNumberFrame(), this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration (100);

  QString fileName;
  bool result;
  mTimerCounter = 0;
  if (mGenerateBeginEndFrames) {
    //First save first frame without route and vehicle (for convenience)
    fileName = dirName + "/" + filePrefix + QString("%1.bmp").arg(mTimerCounter, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0'));
    result = mImage.save (fileName);
    if (!result)
    {
      QMessageBox::critical (this, "Oops", "Problems saving file " + fileName);
      generationOK = false;
    }
    else
    {
      generatedBMPs.append(fileName);
    }
  }


  for (mTimerCounter++; mTimerCounter < mRgr->getNumberFrame() && generationOK; mTimerCounter++)
  {
    progress.setValue(mTimerCounter);
    QPixmap newPixMap(mImage);
    QPainter painter(&newPixMap);
    painter.drawPixmap(0, 0, mImage);
    drawPath(painter);
    //if (i < mRgr->getNumberFrame()) - 1 || !mGenerateBeginEndFrames)
    //  drawVehicle(painter, i);
    QString postFix = QString("%1.bmp").arg(mTimerCounter, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0'));
    fileName = dirName + "/" + filePrefix + postFix;
    result = newPixMap.save (fileName);
    if (!result)
    {
      QMessageBox::critical (this, "Oops", "Problems saving file " + fileName);
      break;
    }
    else
    {
      generatedBMPs.append(fileName);
    }
    if (progress.wasCanceled()) break;
  }	
  generationOK = (mTimerCounter == mRgr->getNumberFrame());

  emit busy(false);

  return generationOK;
}

QSize RGMapWidget::sizeHint () const
{
  return mImage.size();
}

void RGMapWidget::startDrawMode()
{
  if (mInDrawMode) return;
  mInDrawMode = true;
  while (!mUndoBuffer.empty())
    mUndoBuffer.pop();
  update();
  setCursor(Qt::CrossCursor);
  emit canGenerate(false);
  emit drawModeChanged(true);
}

void RGMapWidget::endDrawMode()
{
  if (!mInDrawMode) return;
  mInDrawMode = false;
  setCursor(Qt::ArrowCursor);

  emit drawModeChanged(false);
  update();
}

void RGMapWidget::setPen(const QColor &color,int size,Qt::PenStyle style)
{
    mRgr->setPen(color,size,style);
    update();
}

void RGMapWidget::setGenerateBeginEndFrames(bool val)
{
  mGenerateBeginEndFrames = val;
}

void RGMapWidget::play()
{
  //Disable draw mode automatically
    endDrawMode();

  //Only usefull when route has more than MIN_PATH_LENGTH points
  if (mRgr->stepCount() < MIN_PATH_LENGTH) return;

  emit busy(true);

  mTimerCounter = 0;
  mPlayTimer->start((1.0 / (double) mFPS) * 1000);
  update();
}

void RGMapWidget::stop()
{
  if (mPlayTimer == NULL || !mPlayTimer->isActive()) return;
  //Finished
  mPlayTimer->stop();
  emit busy(false);
  update();
}

void RGMapWidget::paintEvent ( QPaintEvent * event )
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.drawPixmap(0, 0, mImage);
  drawPath(painter);
}

void RGMapWidget::drawPath(QPainter &painter)
{
  painter.setPen (mRgr->getPen());
  if (mInitPhase)
  {
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, applicationName);
  }
  else if (!mInDrawMode)
  {

      //QPainterPath path;
      //path =mRgr->getPathAt(mTimerCounter);
      //painter.drawPath(path);
      mRgr->drawPathAt(mTimerCounter,painter);
      /*if (mVehicle && mRgr->stepCount()>1)
      {
          float angle;
          angle=mRgr->getAngleAt(mTimerCounter);
          QPixmap vehim = mVehicle->getPixmap(-angle);
          //Draw vehicle with center on current point
          int px = path.elementAt(path.elementCount()-1).x - vehim.size().width() / 2;
          int py = path.elementAt(path.elementCount()-1).y - vehim.size().height() / 2;
          painter.drawPixmap(px, py, vehim);
      }*/
  }
  else
      mRgr->drawPath(painter);//painter.drawPath(mRgr->getPath());
}

void RGMapWidget::resizeEvent ( QResizeEvent * event )
{
  QWidget::resizeEvent(event);
  //TODO: Do we need this?
  //mBuffer = QPixmap(event->size());
}

void RGMapWidget::mousePressEvent ( QMouseEvent * event )
{
  if (mInDrawMode && event->button() == Qt::LeftButton)
  {
    qDebug() << "RGMapWidget::mousePressEvent -> " << event->pos();
    mRgr->addPoint(QPoint(event->pos()));
  }
  update();
}

void RGMapWidget::mouseMoveEvent ( QMouseEvent * event )
{
  //Since mouseTracking is off this event only occurs while the mouse button is pressed

  if (mInDrawMode)
  {
      qDebug() << QString::number(event->x()) + "," + QString::number(event->y());
      mRgr->addPoint(QPoint(event->pos()));
   }
  update();

}


void RGMapWidget::mouseReleaseEvent ( QMouseEvent * event ) 
{
  if (mInDrawMode && event->button() == Qt::LeftButton)
  {
    emit canGenerate(mRgr->stepCount() > 1);
    update();
  }
}

void RGMapWidget::playTimerEvent()
{
  if (mTimerCounter < mRgr->getNumberFrame())
  {
    ++mTimerCounter;
  }
  else
  {
    //Finished
    mPlayTimer->stop();
    emit busy(false);
    qDebug()<<"Last update of mTimerCounter"<<mTimerCounter ;
   }

  update();
}

void RGMapWidget::setBusy(bool _busy)
{
  if (_busy)            setCursor(Qt::WaitCursor); 
  else if (mInDrawMode) setCursor(Qt::CrossCursor);
  else                  unsetCursor();
}

void RGMapWidget::undo()
{
  /*if (mUndoBuffer.empty()) return;
  int idx = mUndoBuffer.top();
  while (idx < mPath.size())
    mPath.removeLast();
  mUndoBuffer.pop();
  update();*/
}

void RGMapWidget::redo()
{
  //TODO
}

//Set frame rate of route
void RGMapWidget::setFPS(int fps)
{
  mFPS = fps;
  mRgr->setFPS(fps);
}

//Turns on/off interpolation mode (default is off)
void RGMapWidget::setInterpolationMode(bool val)
{

  RGSettings::setInterpolationMode(val);
  mRgr->setPlayMode((int) val);
}

//The time that the total route animation takes (independent of the route length)
void RGMapWidget::setRoutePlayTime(int time)
{
  mRgr->setTotalTime(time);
  RGSettings::setRoutePlayTime(time);
}


int RGMapWidget::getNoFrames() const
{
  return mRgr->getNumberFrame();
}

void RGMapWidget::startNewRoute()
{

    mRgr->clear();
    emit canGenerate(false);
    update();
}

int RGMapWidget::getMapSize() const
{
  //Returns raw number of bytes of the map
  return mImage.width() * mImage.height() * (mImage.depth() / 8);
}
