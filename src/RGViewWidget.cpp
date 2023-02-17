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

#include "RGViewWidget.h"
#include "RGSettings.h"
#include <QMessageBox>
#include <QProgressDialog>

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

extern const QString applicationName;

RGViewWidget::RGViewWidget(RGMap *map, QWidget *parent) :
  QGraphicsView(parent),
  mPlayTimer(nullptr),
  mTimerCounter(0),
  mNumScheduledScalings(0)
{
  mScene = new QGraphicsScene;
  this->setScene(mScene);
  this->setRenderHint(QPainter::Antialiasing);
  this->setRenderHint(QPainter::SmoothPixmapTransform);
  this->setDragMode(QGraphicsView::ScrollHandDrag);

  //init text
  mWelcomeText = mScene->addText(applicationName,QFont("Arial", 30));
  mWelcomeText->setDefaultTextColor(Qt::blue);

  //timer
  if (mPlayTimer == nullptr) {
    mPlayTimer = new QTimer(this);
    QObject::connect(mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerEvent()));
  }

  //zoom animation
  mAnim = new QTimeLine(350, this);
  mAnim->setUpdateInterval(20);
  connect(mAnim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
  connect(mAnim, SIGNAL (finished()), SLOT (animFinished()));

  if (map)
  {
      QObject::connect(map, &RGMap::mapLoaded,
                       this, &RGViewWidget::loadImage);
  }

}

QSize RGViewWidget::sizeHint () const
{
  return QSize(mScene->width(),mScene->height());
}

void RGViewWidget::addRoute(RGRoute *item)
{
  mRoute=item;
  mScene->addItem(mRoute);
  QObject::connect(this,SIGNAL(sceneRectChanged()),mRoute,SIGNAL(sceneRectChanged()));
}

void RGViewWidget::play()
{
  mTimerCounter = 0;
  mRoute->setCurrentFrame(0);
  mRoute->startPlayback(true);
  mPlayTimer->start((1.0 / (double) RGSettings::getFps()) * 1000);
}

void RGViewWidget::stop()
{
  if (mPlayTimer == nullptr || !mPlayTimer->isActive()) return;
  //Finished
  mPlayTimer->stop();
  mRoute->startPlayback(false);
  emit playbackStopped(true);
}

void RGViewWidget::wheelEvent(QWheelEvent *event)
{    
    //Example from: https://wiki.qt.io/Smooth_Zoom_In_QGraphicsView
    int numDegrees = event->angleDelta().y() / 8;

    int numSteps = numDegrees / 15;
    mNumScheduledScalings += numSteps;
    if (mNumScheduledScalings * numSteps < 0)
    {
        // if user moved the wheel in another direction, we reset previously scheduled scalings
        mNumScheduledScalings = numSteps;
    }

    mAnim->start();
}

bool RGViewWidget::saveRenderedImage(const QString &filename, bool fullMapResolution)
{
    QSize outputResolution = RGSettings::getOutputResolution();
    QRect fullMapRect(0, 0, mScene->width(), mScene->height());
    bool result = false;
    if (fullMapResolution)
    {
        QImage outImage(fullMapRect.size(), QImage::Format_ARGB32);
        QPainter painter(&outImage);
        mScene->render(&painter);
        result = outImage.save(filename);
        painter.end();
    }
    else
    {
        //In case the background map has a higher resolution than the target resolution of the output video,
        //we should create a QImage in that resolution and make sure that the vehicle location is always centered.
        //Then we slide/scroll the background map together, while the vehicle remains centered.
        //If the resolution of background map is identical to output resolution, no scrolling or sliding
        //will happen.
        //Two ways of sliding:
        //1. always try to keep vehicle centered (will result in more sliding): use full map as outer rectangle
        //2. prevent sliding if possible, by first determining the rectangle containing the full route and
        //   use that window as outer rectangle, instead of the full map.

        //TODO: This outerRect calculation only has to be done once, when generating multiple frames, so
        //      do this once and pass the requested outerRect as argument with this method.
        //We try to use method 2. if that fails go for method 1. (could make this a setting later, to always go for method 1)
        QRect outerRect = mRoute->boundingRect().toRect();
        //outerRect += QMargins(15, 15, 15, 15); //Add some margin to give enough space around the route
        qDebug() << "outputResolution             :" << outputResolution;
        qDebug() << "fullMapRect                  :" << fullMapRect;
        qDebug() << "Route outerRect after margins:" << outerRect;
        if (!fullMapRect.contains(outerRect, true))
        {
            //Go for method 1 anyway
            outerRect = fullMapRect;
        }
        else
        {
            //Make sure the outerRect minimally is as large as the requested resolution of the outWindow,
            //by moving the right and left sides of the outerRect until the map boundary is hit.
            //For now the choice is made to always move the rectangle as far top/left as possible.
            if (outerRect.width() < outputResolution.width())
            {
                int remainingCorrection = outputResolution.width() - outerRect.width();
                int correctLeftWidth = fullMapRect.right() - outerRect.right();
                remainingCorrection -= correctLeftWidth;
                outerRect.setLeft(outerRect.left() - correctLeftWidth);
                outerRect.setRight(outerRect.right() + remainingCorrection - correctLeftWidth);
            }
            if (outerRect.height() < outputResolution.height())
            {
                int remainingCorrection = outputResolution.height() - outerRect.height();
                int correctTopHeigth = fullMapRect.top() - outerRect.top();
                remainingCorrection -=correctTopHeigth;
                outerRect.setTop(outerRect.top() - correctTopHeigth);
                outerRect.setBottom(outerRect.bottom() + remainingCorrection - correctTopHeigth);
            }
        }
        qDebug() << "final outerRect              :" << outerRect;

        QPoint vehPos = mRoute->currentVehiclePos().toPoint();
        QRect outWindow(vehPos.x() - 0.5 * outputResolution.width(), vehPos.y() - 0.5 * outputResolution.height(),
                        outputResolution.width(), outputResolution.height());
        if (!outerRect.contains(outWindow, true))
        {
            //Sliding window outside map, reposition inside the map
            QRect intersectRect = outWindow.intersected(outerRect);
            int moveX = outWindow.width() - intersectRect.width();
            int moveY = outWindow.height() - intersectRect.height();
            if (outWindow.bottom() > outerRect.bottom()) moveY = -moveY;
            if (outWindow.right() > outerRect.right()) moveX = -moveX;
            outWindow.translate(moveX, moveY);
        }
        QImage outImage(outWindow.size(), QImage::Format_ARGB32);
        QPainter painter(&outImage);
        mScene->render(&painter, QRectF(), outWindow);
        result = outImage.save(filename);
        painter.end();
    }
    if (!result){
        QMessageBox::critical (this, "Oops", "Problems saving file " + filename);
    }

    return result;
}

bool RGViewWidget::generateMovie(const QString &dirName, const QString &filePrefix, const QString &frameFileType, QStringList &generatedImageFiles)
{
  generatedImageFiles.clear();
  bool generationOK = true;
	int secsBefore = RGSettings::getBeginDelaySeconds();
	int framesBefore = secsBefore * RGSettings::getFps();
	int secsAfter = RGSettings::getEndDelaySeconds();
	int framesAfter = secsAfter * RGSettings::getFps();

  QProgressDialog progress("Generating files...", "Abort", 0, mRoute->countFrames() + framesBefore + framesAfter, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration (100);

	//Generate number of frames before
	mTimerCounter=0;
  mRoute->setCurrentFrame(0);
	for (int frameCounter = 0; frameCounter < framesBefore; ++frameCounter)
	{
        progress.setValue(frameCounter);
        if (!saveFrame(frameCounter, dirName, filePrefix, frameFileType, generatedImageFiles))
        {
            return false;
        }
        if (progress.wasCanceled()) return false;
	}

	//Generate the animated frames
  for (mTimerCounter=0; mTimerCounter < mRoute->countFrames() && generationOK; mTimerCounter++)
	{
    progress.setValue(framesBefore + mTimerCounter);
    mRoute->setCurrentFrame(mTimerCounter);
    if (!saveFrame(framesBefore + mTimerCounter, dirName, filePrefix, frameFileType, generatedImageFiles))
		{
      break;
    }
    if (progress.wasCanceled()) break;
  }

	//Generate number of static frames after
	for (int frameCounter = 0; frameCounter < framesAfter; ++frameCounter)
	{
        progress.setValue(framesBefore + mTimerCounter + frameCounter);
        if (!saveFrame(framesBefore + mTimerCounter + frameCounter, dirName, filePrefix, frameFileType, generatedImageFiles))
        {
            return false;
        }
        if (progress.wasCanceled()) return false;
	}

  generationOK = (mTimerCounter == mRoute->countFrames());

  return generationOK;
}

void RGViewWidget::loadImage(const QPixmap &pm)
{
  if(mWelcomeText!=nullptr){
    delete mWelcomeText;
    mWelcomeText=nullptr;
  }
  mScene->setBackgroundBrush(pm);
  this->setCacheMode(QGraphicsView::CacheBackground);
  mScene->setSceneRect(0,0,pm.width(),pm.height());
  emit sceneRectChanged();
  this->setFrameShape(QFrame::NoFrame);
  viewport()->setMaximumSize(pm.size());
  this->setAlignment((Qt::AlignLeft | Qt::AlignTop));
  updateGeometry();
}

void RGViewWidget::playTimerEvent()
{
  ++mTimerCounter;
  if (mTimerCounter < mRoute->countFrames()){
    mRoute->setCurrentFrame(mTimerCounter);
  }
  else
  {
    //Finished
    this->stop();
  }
}

bool RGViewWidget::saveFrame(int frameCounter, const QString &dirName, const QString &filePrefix, const QString &frameFileType, QStringList &generatedImages)
{
	QString fileName;
    QString postFix = QString("%1.%2").arg(frameCounter, FILE_NUMBER_FIELD_WIDTH, 10, QChar('0')).arg(frameFileType);
	fileName = dirName + "/" + filePrefix + postFix;
	if (!saveRenderedImage(fileName))
	{
		return false;
	}
    else
    {
        generatedImages.append(fileName);
	}
    return true;
}

void RGViewWidget::scalingTime(qreal /* x */)
{
    //Prevent zooming out below 1.0
    qreal factor = 1.0+ qreal(mNumScheduledScalings) / 300.0;
    if (factor * transform().m11() >= 1.0)
    {
        scale(factor, factor);
    }
}

void RGViewWidget::animFinished()
{
    if (mNumScheduledScalings > 0)
    {
        mNumScheduledScalings--;
    }
    else
    {
        mNumScheduledScalings++;
    }
}

