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
#include "RGRoute.h"

class RGViewWidget : public QGraphicsView
{
  Q_OBJECT
public:
  explicit RGViewWidget(QWidget *parent = 0);
  QSize   sizeHint () const;
  void addRoute(RGRoute*);
  bool saveRenderedImage(const QString &filename);
  bool generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs);

signals:
  void playbackStopped(bool);
  void sceneRectChanged();

public slots:
  void loadImage(const QPixmap &pm);
  void play();
  void stop();

private slots:
  void playTimerEvent();

private:
  QGraphicsScene *mScene;
  QGraphicsTextItem *mWelcomeText;
  RGRoute *mRoute;
  QTimer        *mPlayTimer;
  int           mTimerCounter;

};

#endif // RGVIEWWIDGET_H
