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


#ifndef RGMAP_WIDGET_H
#define RGMAP_WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QTimer>
#include <QStringList>
#include <stack>

#include "RGVehicle.h"

class QCheckBox;

class RGRoute;

class RGMapWidget : public QWidget
{
Q_OBJECT

public:
	RGMapWidget(QWidget *parent = 0);

	QPixmap getImage();
	QPixmap getVehiclePixmap();
	bool    generateMovie(const QString &dirName, const QString &filePrefix, QStringList &generatedBMPs);
	int     getMapSize() const;  //Returns number of bytes that the map takes 
	int     getNoFrames() const; //Returns number of frames in the route
	QColor  getPenColor() const;
	
	QSize sizeHint () const;

signals:
	void busy(bool);
	void canGenerate(bool);
  void drawModeChanged(bool activated);

public slots:
	void loadImage(const QPixmap &pm);
	void setVehicle(const QString &fileName, bool mirror, int startAngle);
	void startDrawMode();
	void endDrawMode();
	void setPenSize(int);
	void setPenColor(const QColor &col);
	void setPenStyle(const Qt::PenStyle style);
	void setGenerateBeginEndFrames(bool);
	void play();
	void stop();
	void setBusy(bool);
  void undo();
  void redo();
  
  //Change frame rate of generated movie
  void setFPS(int fps);
  //Turns on/off interpolation mode (default is off)
  void setInterpolationMode(bool val);
  //The time that the total route animation takes (independent of the route length)
  //Only used in interpolation mode!
  void setRoutePlayTime(int time);

protected:
	void paintEvent ( QPaintEvent * event );
	void resizeEvent ( QResizeEvent * event );

	void mousePressEvent ( QMouseEvent * event ) ;
	void mouseMoveEvent ( QMouseEvent * event ) ;
	void mouseReleaseEvent ( QMouseEvent * event ) ;

private slots:
	void playTimerEvent();

private:
	void drawPath(QPainter &painter);
  void drawVehicle(QPainter &painter, int idx);

private:
	//The image on which we paint
	QPixmap       mImage;

	//The icon that's drawn at the end of the path (e.g. train, bike)
	RGVehicle    *mVehicle;

	//The path itself
	QList<QPoint>    mPath;
  std::stack<int>  mUndoBuffer;
  RGRoute         *mRgr;

	//Color of the route
	QColor        mPenColor;

  //Line style of the route
  Qt::PenStyle  mPenStyle;

	//Some other settings
	bool          mInDrawMode;
	bool          mInitPhase;
	bool          mGenerateBeginEndFrames;
  bool          mInterpolationMode;
	int           mPenSize;
  int           mFPS;      //Frames per second (directly related to mInterval)
  int           mRoutePlayTime; //(in seconds)

	//We use this for preview and generating movie only
	QTimer        *mPlayTimer;
	QList<QPoint> mPathBackup; 
	int           mSkip;
	int           mTimerCounter;

};

#endif //MAP_WIDGET_H