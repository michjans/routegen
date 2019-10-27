/*
    Copyright (C) 2008-2011  Michiel Jansen

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
#include <QHash>
#include <RGSettings.h>

QString RGSettings::getVideoEncoder()
{
  QSettings settings;
  return settings.value("videoEncoder", "ffmpeg").toString();
}

void RGSettings::setVideoEncoder(const QString &exec)
{
  QSettings settings;
  settings.setValue("videoEncoder", exec);
}

QString RGSettings::getVideoEncExec()
{
  QSettings settings;
#ifdef __linux__
  return settings.value("videoEncExec", "/usr/bin/ffmpeg").toString();
#else
  return settings.value("videoEncExec", QDir::currentPath() + "/ffmpeg/bin/ffmpeg.exe").toString();
#endif
}

void RGSettings::setVideoEncExec(const QString &exec)
{
  QSettings settings;
  settings.setValue("videoEncExec", exec);
}

QString RGSettings::getAviOutName()
{
  QSettings settings;
  return settings.value("videoArgOutname", QString("out")).toString();
}

void RGSettings::setAviOutName(const QString &outname)
{
  QSettings settings;
  settings.setValue("videoArgOutname", outname);
}

int RGSettings::getFps()
{
  QSettings settings;
  return settings.value("videoArgFps", QString("25")).toInt();
}

void RGSettings::setFps(int fps)
{
  QSettings settings;
  settings.setValue("videoArgFps", fps);
  //TODO: When FPS is changed, RGMapWidget should be notified somehow!!!!!
}

int RGSettings::getKeyFrameRate()
{
  QSettings settings;
  return settings.value("videoArgKeyFrameRate",QString("25")).toInt();
}

void RGSettings::setKeyFrameRate(int rate)
{
  QSettings settings;
  settings.setValue("videoArgKeyFrameRate", rate);
}

int RGSettings::getBitRate()
{
  QSettings settings;
  return settings.value("videoArgBitRate",QString("1500")).toInt();
}

void RGSettings::setBitRate(int bitrate)
{
  QSettings settings;
  settings.setValue("videoArgBitRate", bitrate);
}

QString RGSettings::getAviCompression()
{
  QSettings settings;
  return settings.value("videoArgCompression",QString("")).toString();
}

void RGSettings::setAviCompression(const QString &comp)
{
  QSettings settings;
  settings.setValue("videoArgCompression", comp);
}

bool RGSettings::getManualCommandLineChecked()
{
    QSettings settings;
    return settings.value("manualCommandLineChecked", false).toBool();
}

void RGSettings::setManualCommandLineChecked(bool val)
{
    QSettings settings;
    settings.setValue("manualCommandLineChecked", val);
}

QString RGSettings::getFFMpegCommandlineArgs()
{
    QSettings settings;
    return settings.value("ffMpegCommandlineArgs",QString("")).toString();
}

void RGSettings::setFFMpegCommandlineArgs(const QString &args)
{
    QSettings settings;
    settings.setValue("ffMpegCommandlineArgs", args);
}

bool RGSettings::getDeleteBMPs()
{
  QSettings settings;
  return settings.value("videoDeleteBMPs", false).toBool();
}

void RGSettings::setDeleteBMPs(bool val)
{
  QSettings settings;
  settings.setValue("videoDeleteBMPs", val);
}

bool RGSettings::getIconLessBeginEndFrames()
{
  QSettings settings;
  return settings.value("iconLessBeginEndFrames", false).toBool();
}

void RGSettings::setIconLessBeginEndFrames(bool val)
{
  QSettings settings;
  settings.setValue("iconLessBeginEndFrames", val);
}

int RGSettings::getBeginDelaySeconds()
{
  QSettings settings;
  return settings.value("beginDelaySeconds", QString("0")).toInt();
}

void RGSettings::setBeginDelaySeconds(int val)
{
  QSettings settings;
  settings.setValue("beginDelaySeconds", val);
}

int RGSettings::getEndDelaySeconds()
{
  QSettings settings;
  return settings.value("endDelaySeconds", QString("0")).toInt();
}

void RGSettings::setEndDelaySeconds(int val)
{
  QSettings settings;
  settings.setValue("endDelaySeconds", val);
}

QRectF RGSettings::getMapGeoBounds(const QString &fileName)
{
    QSettings settings;

    QHash<QString, QVariant> geoBoundMap = settings.value("geoBounds", QHash<QString, QVariant>()).toHash();
    return geoBoundMap[fileName].toRectF();
}

void RGSettings::setMapGeoBounds(const QString &fileName, const QRectF &geoBounds)
{
    QSettings settings;

    QHash<QString, QVariant> geoBoundMap = settings.value("geoBounds", QHash<QString, QVariant>()).toHash();
    geoBoundMap[fileName] = QVariant(geoBounds);

    settings.setValue("geoBounds", geoBoundMap);
}

QColor RGSettings::getPenColor()
{
  QSettings settings;
  settings.beginGroup("routeColor");
  QColor penCol(settings.value("red",   0).toInt(),
                settings.value("green", 0).toInt(),
                settings.value("blue",  255).toInt());
  settings.endGroup();

  return penCol;
}

void RGSettings::setPenColor(const QColor &newCol)
{
  QSettings settings;
  settings.beginGroup("routeColor");
  settings.setValue("red", newCol.red());
  settings.setValue("green", newCol.green());
  settings.setValue("blue", newCol.blue());
  settings.endGroup();
}


int RGSettings::getPenSize()
{
  QSettings settings;
  return settings.value("routeWidth", 5).toInt();
}

void RGSettings::setPenSize(int size)
{
  QSettings settings;
  settings.setValue("routeWidth", size);
}

int RGSettings::getPenStyle()
{
  QSettings settings;
  return settings.value("routeStyle", 1).toInt();
}

void RGSettings::setPenStyle(int style)
{
  QSettings settings;
  settings.setValue("routeStyle", style);
}

QString RGSettings::getLastOpenDir(FileLocation loc)
{
  QSettings settings;
  switch (loc)
  {
  case RG_GPX_LOCATION:
      return settings.value("lastGPXDir", QDir::homePath()).toString();
  case RG_MOVIE_LOCATION:
      return settings.value("lastGenDir", QDir::homePath()).toString();
  case RG_PROJECT_LOCATION:
      return settings.value("lastProjDir", QDir::homePath()).toString();
  case RG_MAP_LOCATION:
  default:
      return settings.value("lastOpenDir", QDir::homePath()).toString();
  }
}

void RGSettings::setLastOpenDir(const QString &fileName, FileLocation loc)
{
  QSettings settings;
  switch (loc)
  {
  case RG_GPX_LOCATION:
      settings.setValue("lastGPXDir", fileName);
      break;
  case RG_MOVIE_LOCATION:
      settings.setValue("lastGenDir", fileName);
      break;
  case RG_PROJECT_LOCATION:
      settings.setValue("lastProjDir", fileName);
      break;
  case RG_MAP_LOCATION:
  default:
      settings.setValue("lastOpenDir", fileName);
  }
}

QString RGSettings::getLastVehicleName()
{
  QSettings settings;
  return settings.value("lastVehicleName", "None").toString();
}

void RGSettings::setLastVehicleName(const QString &name)
{
  QSettings settings;
  settings.setValue("lastVehicleName", name);
}

int RGSettings::getVehicleAngle(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleAngles");
  int angle = settings.value(name, 0).toInt();
  settings.endGroup();
  return angle;
}

void RGSettings::setVehicleAngle(const QString &name, int angle)
{
  QSettings settings;
  settings.beginGroup("vehicleAngles");
  settings.setValue(name, angle);
  settings.endGroup();
}

int RGSettings::getVehicleSize(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleSizes");
  int size = settings.value(name, 0).toInt();
  settings.endGroup();
  return size;
}

void RGSettings::setVehicleSize(const QString &name, int size)
{
  QSettings settings;
  settings.beginGroup("vehicleSizes");
  settings.setValue(name, size);
  settings.endGroup();
}

bool RGSettings::getVehicleMirrored(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleMirror");
  bool mirrored = settings.value(name, false).toBool();
  settings.endGroup();
  return mirrored;
}

void RGSettings::setVehicleMirrored(const QString &name, bool mirror)
{
  QSettings settings;
  settings.beginGroup("vehicleMirror");
  settings.setValue(name, mirror);
  settings.endGroup();
}

QPointF RGSettings::getVehicleOrigin(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleOriginX");
  float pointx= settings.value(name, -1).toFloat();
  settings.endGroup();
  settings.beginGroup("vehicleOriginY");
  float pointy= settings.value(name, -1).toFloat();
  settings.endGroup();
  return QPointF(pointx,pointy);
}

void RGSettings::setVehicleOrigin(const QString &name, QPointF point)
{
  QSettings settings;
  settings.beginGroup("vehicleOriginX");
  settings.setValue(name, point.x());
  settings.endGroup();
  settings.beginGroup("vehicleOriginY");
  settings.setValue(name, point.y());
  settings.endGroup();
}

bool RGSettings::getVehicleAcceptsRotation(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleAcceptsRotation");
  bool accept = settings.value(name, true).toBool();
  settings.endGroup();
  return accept;
}

void RGSettings::setVehicleAcceptsRotation(const QString &name, bool accept)
{
  QSettings settings;
  settings.beginGroup("vehicleAcceptsRotation");
  settings.setValue(name, accept);
  settings.endGroup();
}


void RGSettings::setTotalTimeMode(bool val)
{
  QSettings settings;
  settings.setValue("totalTimeMode", val);
}

bool RGSettings::getTotalTimeMode()
{
  QSettings settings;
  return settings.value("totalTimeMode", true).toBool();
}

void RGSettings::setRoutePlayTime(int time)
{
  QSettings settings;
  settings.setValue("routePlayTime", time);
}

int RGSettings::getRoutePlayTime()
{
  QSettings settings;
  return settings.value("routePlayTime", 5).toInt();
}

void RGSettings::setGMXResolution(int xres)
{
  QSettings settings;
  settings.setValue("gmXResolution", xres);
}

int RGSettings::getGMXResolution()
{
  QSettings settings;
  return settings.value("gmXResolution", 768).toInt();
}

void RGSettings::setGMYResolution(int yres)
{
  QSettings settings;
  settings.setValue("gmYResolution", yres);
}

int RGSettings::getGMYResolution()
{
  QSettings settings;
  return settings.value("gmYResolution", 576).toInt();
}

void RGSettings::setSmoothPathMode(bool enable)
{
  QSettings settings;
  settings.setValue("SmoothPathMode", enable);
}

bool RGSettings::getSmoothPathMode(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return false;
  else
    return settings.value("SmoothPathMode", true).toBool();
}

void RGSettings::setSmoothLength(int val)
{
  QSettings settings;
  settings.setValue("SmoothLength", val);
}

int RGSettings::getSmoothLength(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return 10;
  else
    return settings.value("SmoothLength",10).toInt();
}
