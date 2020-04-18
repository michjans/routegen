/*
    Copyright (C) 2009-2011  Michiel Jansen

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
#include <QPointF>
#include <QRectF>
#include <QRect>

class RGSettings
{
public:
    enum FileLocation
    {
        RG_MAP_LOCATION,
        RG_GPX_LOCATION,
        RG_MOVIE_LOCATION,
        RG_PROJECT_LOCATION
    };

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
    static int getBitRate();
    static void setBitRate(const int);
    static QString getAviCompression();
    static void setAviCompression(const QString &);
    static bool getManualCommandLineChecked();
    static void setManualCommandLineChecked(bool val);
    static QString getFFMpegCommandlineArgs();
    static void setFFMpegCommandlineArgs(const QString &args);


    static bool getDeleteBMPs();
    static void setDeleteBMPs(bool val);
    static bool getIconLessBeginEndFrames();
    static void setIconLessBeginEndFrames(bool val);
    static int getBeginDelaySeconds();
    static void setBeginDelaySeconds(int val);
    static int getEndDelaySeconds();
    static void setEndDelaySeconds(int val);

    /*
    * Application settings
    */
    static QRectF getMapGeoBounds(const QString &fileNme);
    static void setMapGeoBounds(const QString &fileNme, const QRectF &geoBounds);
    static QRect getMainWindowGeometry();
    static void setMainWindowGeometry(const QRect &geometry);
    static QColor getPenColor();
    static void setPenColor(const QColor &);
    static int getPenSize();
    static void setPenSize(int);
    static int getPenStyle();
    static void setPenStyle(int);
    static QString getLastOpenDir(FileLocation loc);
    static void setLastOpenDir(const QString &fileName, FileLocation loc);
    static QString getLastVehicleName();
    static void setLastVehicleName(const QString &name);
    static int getVehicleAngle(const QString &name);
    static void setVehicleAngle(const QString &name, int angle);
    static int getVehicleSize(const QString &name);
    static void setVehicleSize(const QString &name, int size);
    static bool getVehicleMirrored(const QString &name);
    static void setVehicleMirrored(const QString &name, bool mirror);
    static QPointF getVehicleOrigin(const QString &name);
    static void setVehicleOrigin(const QString &name, QPointF point);
    static bool getVehicleAcceptsRotation(const QString &name);
    static void setVehicleAcceptsRotation(const QString &name, bool rotate);

    static void setTotalTimeMode(bool val);
    static bool getTotalTimeMode();
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
    static void setSmoothPathMode(bool enable);
    static bool getSmoothPathMode(bool defaultVal = false);
    static void setSmoothLength(int val);
    static int  getSmoothLength(bool defaultVal = false);

};

#endif
