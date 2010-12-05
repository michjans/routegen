/*
    Copyright (C) 2009  Michiel Jansen

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

#ifndef RG_SETTINGS_H
#define RG_SETTINGS_H

#include <QColor>
#include <QString>

class RGSettings
{
public:
  static void initSettings();

  /*
   * Movie generation settings
   */
  static QString getVideoEncoder();
  static void setVideoEncoder(const QString &);
  static QString getVideoEncExec();
  static void setVideoEncExec(const QString &);
  static QString getAviOutName();
  static void setAviOutName(const QString &);
  static int getFps();
  static void setFps(int);
  static int getKeyFrameRate();
  static void setKeyFrameRate(int);
  static QString getAviCompression();
  static void setAviCompression(const QString &);
  static bool getDeleteBMPs();
  static void setDeleteBMPs(bool val);

  /*
   * Application settings
   */
  static QColor getPenColor();
  static void setPenColor(const QColor &);
  static int getPenSize();
  static void setPenSize(int);
  static int getPenStyle();
  static void setPenStyle(int);
  static QString getLastOpenDir();
  static void setLastOpenDir(const QString &fileName);
  static QString getLastSaveDir();
  static void setLastSaveDir(const QString &fileName);
  static QString getLastGenDir();
  static void setLastGenDir(const QString &fileName);
  static int getVehicleAngle(const QString &name);
  static void setVehicleAngle(const QString &name, int angle);
  static int getVehicleSize(const QString &name);
  static void setVehicleSize(const QString &name, int size);
  static bool getVehicleMirrored(const QString &name);
  static void setVehicleMirrored(const QString &name, bool mirror);
  
  static void setInterpolationMode(bool val);
  static bool getInterpolationMode();
  static void setRoutePlayTime(int time);
  static int  getRoutePlayTime();

  //Google maps resolution
  static void setGMXResolution(int xres);
  static int  getGMXResolution();
  static void setGMYResolution(int xres);
  static int  getGMYResolution();

  /*
   * Advanced settings
   */

  //Enable experimental curve interpolation for routes?
  //(Still needs to be fixed, so made experimental)
  static void setCurvedInterpolation(bool enable);
  static bool getCurvedInterpolation(bool defaultVal = false);
  static void setCurveRadius(int val);
  static int  getCurveRadius(bool defaultVal = false);

  //Advanced parameters for calculating vehicle orientation
  static void setVehicleStepAngle(int angle);
  static int  getVehicleStepAngle(bool defaultVal = false);
  static void setVehicleMinDistance(int dist);
  static int  getVehicleMinDistance(bool defaultVal = false);
  static void setVehicleForceCounter(int ctr);
  static int  getVehicleForceCounter(bool defaultVal = false);


};

#endif
