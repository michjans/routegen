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

#ifndef RGVIEWWIDGET_H
#define RGVIEWWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QTimeLine>

#include "RGMap.h"
#include "RGRoute.h"

class RGViewWidget : public QGraphicsView
{
  Q_OBJECT
public:
  explicit RGViewWidget(RGMap *map, QWidget *parent = 0);
  QSize   sizeHint () const;
  void addRoute(RGRoute*);

  /**
   * @brief Saves the currently rendered image
   * @param filename of the image
   * @param mapResolution if the map will be saved in it's full original resolution, otherwise it will
   *                      be saved in the output resolution (sliding window)
   * @return
   */
  bool saveRenderedImage(const QString &filename, bool fullMapResolution = false);
  bool generateMovie(const QString &dirName, const QString &filePrefix, const QString &frameFileType, QStringList &generatedImageFiles);

signals:
  void playbackStopped(bool);
  void sceneRectChanged();

public slots:
  void loadImage(const QPixmap &pm);
  void play();
  void stop();

protected:
   void wheelEvent ( QWheelEvent * event ) override;

private slots:
  void playTimerEvent();
  bool saveFrame(int frameCounter, const QString &dirName, const QString &filePrefix, const QString &frameFileType, QStringList &generatedBMPs);
  void scalingTime(qreal x);
  void animFinished();

private:
  QGraphicsScene *mScene;
  QGraphicsTextItem *mWelcomeText;
  RGRoute *mRoute;
  QTimer        *mPlayTimer;
  QTimeLine     *mAnim;
  int           mTimerCounter;
  int           mNumScheduledScalings;
  QPoint        mDragOrigin;
  qreal         mMinTransformM11;
};

#endif // RGVIEWWIDGET_H
