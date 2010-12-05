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

#ifndef RG_VEHICLE_H
#define RG_VEHICLE_H

#include <vector>
#include <QString>
#include <QImage>
#include <QPoint>
#include <QPixmap>

class RGVehicle
{
public:
  /**
   * Creates a new RGVehicle object.
   *   fileName = full path to image
   *   mirror = mirror icon initially
   *   startAngle = angle to rotate icon initially
   *   defaultDelay = if image is animated gif: use this delay between frames (in ms), 
   *                             when property not available in animation (default = 80, 2 frames if 25 fps)
   */
  RGVehicle(const QString &fileName, bool mirror = false, int startAngle = 0, int size = 0, int defaultDelay = 80);

  ~RGVehicle();

  /**
    * Sets the angle step before calculating and returning new pixmap in getPixmap,
    * if within stepAngle a cached pixmap is returned (for performance reasons)
    * (default = 12)
    */
  void setStepAngle(int stepAngle);

  /**
    * Sets the minimum distance between two points, before calculating a new
    * angle. Each point is stored, and the a new angle between points is calculate
    * new point comes in with a greater distance to last stored point, than the value set here.
    * (default = 10)
    */
  void setMinDistance(int distance);

  /**
    * Sets the maximum number of frames, before calculating a new vehicle angle,
    * independent of minimum distance or step angle.
    * NOTE: Should be higher than min distance!
    * (default = 20)
    */
  void setForceCounter(int forceCtr);
  
  /**
   * Sets the begin point of the route for the vehicle
   */
  void setStartPoint(const QPoint &from);
    
  /**
	*Returns pixmap for the time, passed in milliseconds and with rotation depending on angle
	*between start (set above) and to point and each following point after that. Only calculates
	*a new angle if the next point has a greater distance than the manhatan lenght, set using
	*setManhattanLenght();
	*   time = time in ms, to get next frame for (if animated gif, else always returns same pixmap)
	*   from = coordinates of start point
	*   to      = coordinates of to point
    */
  QPixmap getPixmap(int time, const QPoint &to);

  /**
   *Just returns the first pixmap without angle.
   */
  QPixmap getPixmap();

private:
  QImage rotateImage(QImage &image, int degrees);
  QImage scaleImage(QImage &image, int size);

private:
  std::vector<QImage> mImages;
  int                 mFrameDelay;   //INT_MAX means, no animation
  int                 mStepAngle;    //Minimum angle difference, before rotating the image
  int                 mMinDistance;  //Minimum distance before calculating new angle
  int                 mForceCounter; //After this number always force a new pixmap
  
  QPoint              mCachedPoint;
  int                 mCachedAngle;
  bool                mCachedMirrorRequired;
  int                 mCachedTime;
  bool                mCacheInitialized;
  QPixmap             mCachedPm;
  int                 mCachedCounter;
};

#endif
