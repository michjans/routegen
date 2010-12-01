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

#include "RGMapWidget.h"
#include "RGRoute.h"
#include "RGSettings.h"

#define MIN_PATH_LENGTH 5

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

//Defined in main.cpp
extern const QString applicationName;


RGMapWidget::RGMapWidget(QWidget *parent)
:QWidget(parent),
 mVehicle(NULL),
 mRgr(NULL),
 mPenColor(Qt::blue),
 mPenStyle(Qt::SolidLine),
 mInDrawMode(false),
 mInitPhase(true),
 mGenerateBeginEndFrames(false),
 mInterpolationMode(false),
 mFPS(25),
 mRoutePlayTime(5),  //5 seconds by default (only used by in interpolation mode)
 mPlayTimer(NULL)
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  mFPS = RGSettings::getFps();
  mRoutePlayTime = RGSettings::getRoutePlayTime();
  mInterpolationMode = RGSettings::getInterpolationMode();
}

void RGMapWidget::loadImage(const QPixmap &pm)
{
  mImage = pm;
  mInitPhase = false;
  update();
  updateGeometry();
  //adjustSize();
}

void RGMapWidget::setVehicle(const QString &fileName, bool mirror, int startAngle)
{
  if (mVehicle != NULL) delete mVehicle;
  if (!fileName.isNull()) {
    mVehicle = new RGVehicle(fileName, mirror, startAngle);

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
  if (mInterpolationMode && mInDrawMode)
    endDrawMode();

  generatedBMPs.clear();
  //Only usefull when route has more than MIN_PATH_LENGTH points
  if (mPath.count() < MIN_PATH_LENGTH) {
    QMessageBox::warning (this, "Route too short", "Sorry, this route is too short to generate a valid route.");
    return false;
  }

  bool generationOK = true;
  emit busy(true);
  QString text = QString("Number of frames: ") + QString::number(mPath.count());

  mPathBackup = mPath;
  mPath.clear();

  QProgressDialog progress("Generating files...", "Abort", 0, mPathBackup.count(), this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration (100);

  QString fileName;
  bool result;
  int i = 0;
  if (mGenerateBeginEndFrames) {
    //First save first frame without route and vehicle (for convenience)
    fileName = dirName + "/" + filePrefix + QString("%1.bmp").arg(i, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0'));
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

  if (mVehicle) {
    mVehicle->setStartPoint(mPathBackup[0]);
    //Dummy call to initiate a correct start angle
    //(otherwise it's 0 for the first few frames).
    //We take a point further away to have a better start angle
    mVehicle->getPixmap(0, mPathBackup[MIN_PATH_LENGTH - 1]);
  }


  for (i++; i < mPathBackup.count() && generationOK; i++)
  {
    progress.setValue(i);
    mPath.append(mPathBackup[i]);
    QPixmap newPixMap(mImage);
    QPainter painter(&newPixMap);
    painter.drawPixmap(0, 0, mImage);
    drawPath(painter);
    if (i < mPathBackup.count() - 1 || !mGenerateBeginEndFrames)
      drawVehicle(painter, i);
    QString postFix = QString("%1.bmp").arg(i, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0'));
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
  generationOK = (i == mPathBackup.count());

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
  mPath.clear();
  mPathBackup.clear();
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
  if (mRgr) delete mRgr;
  if (RGSettings::getCurvedInterpolation())
    mRgr = new RGRoute(mPath, RGSettings::getCurveRadius());
  else
    mRgr = new RGRoute(mPath);

#ifdef _DEBUG
  mRgr->dump();
#endif
  //TODO: Only use mPath while drawing, on all places where mPath is used
  //      (e.g. paintEvent), use mRgr to request the raw or interpolated route.
  if (mInterpolationMode)
    mPath = mRgr->getInterpolatedRoute(mFPS * mRoutePlayTime);
  else
    mPath = mRgr->getRawRoute();

  emit drawModeChanged(false);
  update();
}

void RGMapWidget::setPenSize(int size)
{
  mPenSize = size;
  update();
}

void RGMapWidget::setPenStyle(const Qt::PenStyle style)
{
  mPenStyle = style;
  update();
}

void RGMapWidget::setPenColor(const QColor &col)
{
  mPenColor = col;
  update();
}


void RGMapWidget::setGenerateBeginEndFrames(bool val)
{
  mGenerateBeginEndFrames = val;
}

void RGMapWidget::play()
{
  //Disable draw mode automatically
  if (mInterpolationMode && mInDrawMode)
    endDrawMode();

  //Only usefull when route has more than MIN_PATH_LENGTH points
  if (mPath.count() < MIN_PATH_LENGTH) return;

  emit busy(true);
  if (mPlayTimer == NULL) {
    mPlayTimer = new QTimer(this);
    QObject::connect(mPlayTimer, SIGNAL(timeout()),
                   this, SLOT(playTimerEvent()));
  }

  mSkip = 1;
  mPathBackup = mPath;
  mPath.clear();
  mTimerCounter = 0;
  if (mVehicle) {
    mVehicle->setStartPoint(mPathBackup[0]);
    //Dummy call to initiate a correct start angle
    //(otherwise it's 0 for the first few frames).
    //We take a point further away to have a better start angle
    mVehicle->getPixmap(0, mPathBackup[MIN_PATH_LENGTH - 1]);
  }

  //Convert fps to interval time in ms
  mPlayTimer->start((1.0 / (double) mFPS) * 1000);
  update();
}

void RGMapWidget::stop()
{
  if (mPlayTimer == NULL || !mPlayTimer->isActive()) return;
  //Finished
  mPlayTimer->stop();
  mPath = mPathBackup;
  emit busy(false);
  update();
}

void RGMapWidget::paintEvent ( QPaintEvent * event )
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.drawPixmap(0, 0, mImage);
  drawPath(painter);
  if (!mInDrawMode || (mPlayTimer != NULL && mPlayTimer->isActive()))
    drawVehicle(painter, mPath.count());

#ifdef RGROUTE_DEBUG
  //TEST
  if (mRgr)
  {
    QBrush brush(Qt::red, Qt::SolidPattern);
    QPen pen(brush, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen (pen);

    const std::vector<RGRouteSegment *>& segments = mRgr->getSegments();
    std::vector<RGRouteSegment *>::const_iterator it;
    for (it = segments.begin(); it != segments.end(); it++)
    {
      RGLineSegment *rgline = dynamic_cast<RGLineSegment*>(*it);
      if (rgline) {
        pen.setColor(Qt::green);
        painter.setPen(pen);
        painter.drawLine ( rgline->getX1(), rgline->getY1(), rgline->getX2(), rgline->getY2());
      } else {
        pen.setColor(Qt::red);
        painter.setPen(pen);
        RGCircleSegment *rgcircle = dynamic_cast<RGCircleSegment*>(*it);
        if (!rgcircle)
        {
          qDebug() << "WEIRD!!!!!!!!";
          break;
        }
        painter.drawEllipse ( rgcircle->getXm() - rgcircle->getR(), rgcircle->getYm() - rgcircle->getR(),
                              rgcircle->getR() * 2, rgcircle->getR() * 2) ;
      }
    }
  }
#endif
}

void RGMapWidget::drawPath(QPainter &painter)
{
  if (mInitPhase)
  {
    painter.setPen(mPenColor);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, applicationName);
  }
  else if (mPath.count() > 1)
  {
    QPainterPath pPath;
    QBrush brush(mPenColor, Qt::SolidPattern);
    QPen pen(brush, mPenSize, mPenStyle, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen (pen);
    QList<QPoint>::iterator i;
    QPoint prev;
    for (i = mPath.begin(); i != mPath.end(); ++i)
    {
      if (i == mPath.begin()) 
        pPath.moveTo(*i);
      else
        pPath.lineTo(*i);
    }
    painter.drawPath(pPath);
  }
  else if (mInterpolationMode && mInDrawMode && mPath.count() == 1)
  {
    //Visualize start point
    QBrush brush(mPenColor, Qt::SolidPattern);
    QPen pen(brush, mPenSize, mPenStyle, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen (pen);
    painter.drawPoint(mPath[0]);
  }
}

//Draws vehicle on point idx in mPathBackup list
void RGMapWidget::drawVehicle(QPainter &painter, int idx)
{
  if (mVehicle == NULL  || mPathBackup.count() == 0) return;

  //Use mPathBackup, because mPath contains the path until now and
  //mPathBackup contains all point
  //(drawVehicle can be called with a to big index, in this case, just use the last point)
  if (idx >= mPathBackup.count()) idx = mPathBackup.count() - 1;
  QPoint point = mPathBackup[idx];

  //Time between frames in ms.
  int interval = (1.0 / (double) mFPS) * 1000;

  QPixmap vehim = mVehicle->getPixmap(idx * interval, point);

  //Draw vehicle with center on current point
  int px = point.x() - vehim.size().width() / 2;
  int py = point.y() - vehim.size().height() / 2;
  painter.drawPixmap(px, py, vehim);

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
    mPath.append(event->pos());
    mUndoBuffer.push(mPath.size() - 1);
  }

  if (mInterpolationMode)
    update();
}

void RGMapWidget::mouseMoveEvent ( QMouseEvent * event )
{
  //Since mouseTracking is off this event only occurs while the mouse button is pressed

  qDebug() << QString::number(event->x()) + "," + QString::number(event->y());

  //Only in raw mode (not interpolation) the user can drag the route line
  if (mInDrawMode && !mInterpolationMode)
  {
    mPath.append(event->pos());
    update();
  }
}


void RGMapWidget::mouseReleaseEvent ( QMouseEvent * event ) 
{
  if (mInDrawMode && event->button() == Qt::LeftButton)
  {
    emit canGenerate(mPath.count() > 0);
    update();
  }
}

void RGMapWidget::playTimerEvent()
{
  if (mTimerCounter < mPathBackup.count())
  {
    mPath.append(mPathBackup[mTimerCounter]);
    ++mTimerCounter;
  }
  else
  {
    //Finished
    mPlayTimer->stop();
    emit busy(false);
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
  if (mUndoBuffer.empty()) return;
  int idx = mUndoBuffer.top();
  while (idx < mPath.size())
    mPath.removeLast();
  mUndoBuffer.pop();
  update();
}

void RGMapWidget::redo()
{
  //TODO
}

//Set frame rate of route
void RGMapWidget::setFPS(int fps)
{
  mFPS = fps;
  if (mRgr && mInterpolationMode)
    mPath = mRgr->getInterpolatedRoute(mFPS * mRoutePlayTime);
}

//Turns on/off interpolation mode (default is off)
void RGMapWidget::setInterpolationMode(bool val)
{
  if (mInterpolationMode == val) return;
  mInterpolationMode = val;
  //The route should be drawn differently, so clear existing route
  mPath.clear();
  if (mRgr) {
    delete mRgr;
    mRgr = 0;
  }
  RGSettings::setInterpolationMode(val);
}

//The time that the total route animation takes (independent of the route length)
void RGMapWidget::setRoutePlayTime(int time)
{
  if (mRoutePlayTime == time) return;
  mRoutePlayTime = time;
  if (mRgr && mInterpolationMode)
    mPath = mRgr->getInterpolatedRoute(mFPS * mRoutePlayTime);
  RGSettings::setRoutePlayTime(time);
}


int RGMapWidget::getNoFrames() const
{
  return mPath.size();
}

QColor RGMapWidget::getPenColor() const
{
  return mPenColor;
}

int RGMapWidget::getMapSize() const
{
  //Returns raw number of bytes of the map
  return mImage.width() * mImage.height() * (mImage.depth() / 8);
}
