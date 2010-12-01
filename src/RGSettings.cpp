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

#include <QtGui>

#include <RGSettings.h>


void RGSettings::initSettings()
{
  //Currently we only initialize bmp2avi
  QSettings settings;
  QString bmp2aviExecName = settings.value("bmp2aviExec", QDir::currentPath() + "/bmp2avi/bmp2avi.exe").toString();
  QFile bmp2aviExec(bmp2aviExecName);
  while (bmp2aviExec.exists() == false || !bmp2aviExec.fileName().contains(QString("bmp2avi")))
  {
    //Bmp2avi not found, ask user for different directory
    if (QMessageBox::question (NULL, "Bmp2Avi not found",
                             "Could not find bmp2avi.exe, do you want to browse for it?",
                       QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
      bmp2aviExecName = QFileDialog::getOpenFileName(NULL,
                      QString("Select the directory where bmp2avi.exe is located."),
                      QDir::currentPath(),
                      "Executables (*.exe)");
      bmp2aviExec.setFileName(bmp2aviExecName);
    } else {
      break;
    }
  }

  if (bmp2aviExec.exists())
  //Store new location
  settings.setValue("bmp2aviExec", bmp2aviExecName);
  else
  QMessageBox::warning (NULL, "Not available", "Avi generation using bmp2avi will be unavailable");

  /*
   *Initialize the default command line arguments that we pass to bmp2avi.
   *These will be stored in the (registry) settings, so mutable when required.
   *By default use a frame rate of 25 fps and keyframe rate of 25 frames.
   *The default compression setting chosen by bmp2avi is uncompressed,
   *so the keyframe rate has no effect. But it's possible to choose different
   *compression encoders, using the -c argument.
   */
  QVariant fps      = settings.value("bmp2aviArgFps",          QString("25"));
  QVariant key      = settings.value("bmp2aviArgKeyFrameRate", QString("25"));
  QVariant outname  = settings.value("bmp2aviArgOutname",      QString("out"));
  //Compression "DIB", means none, see bmp2avi documentation
  QVariant compress = settings.value("bmp2aviArgCompression",  QString("DIB"));

  settings.setValue("bmp2aviArgFps", fps);
  settings.setValue("bmp2aviArgKeyFrameRate", key);
  settings.setValue("bmp2aviArgOutname", outname);
  settings.setValue("bmp2aviArgCompression", compress);
}

QString
RGSettings::getBmp2AviExec()
{
  QSettings settings;
  return settings.value("bmp2aviExec").toString();
}

void
RGSettings::setBmp2AviExec(const QString &exec)
{
  QSettings settings;
  settings.setValue("bmp2aviExec", exec);
}

QString
RGSettings::getAviOutName()
{
  QSettings settings;
  return settings.value("bmp2aviArgOutname", QString("out")).toString();
}

void
RGSettings::setAviOutName(const QString &outname)
{
  QSettings settings;
  settings.setValue("bmp2aviArgOutname", outname);
}

int
RGSettings::getFps()
{
  QSettings settings;
  return settings.value("bmp2aviArgFps").toInt();
}

void
RGSettings::setFps(int fps)
{
  QSettings settings;
  settings.setValue("bmp2aviArgFps", fps);
  //TODO: When FPS is changed, RGMapWidget should be notified somehow!!!!!
}

int
RGSettings::getKeyFrameRate()
{
  QSettings settings;
  return settings.value("bmp2aviArgKeyFrameRate").toInt();
}

void
RGSettings::setKeyFrameRate(int rate)
{
  QSettings settings;
  settings.setValue("bmp2aviArgKeyFrameRate", rate);
}

QString
RGSettings::getAviCompression()
{
  QSettings settings;
  return settings.value("bmp2aviArgCompression").toString();
}

void
RGSettings::setAviCompression(const QString &comp)
{
  QSettings settings;
  settings.setValue("bmp2aviArgCompression", comp);
}

bool
RGSettings::getDeleteBMPs()
{
  QSettings settings;
  return settings.value("bmp2aviDeleteBMPs", false).toBool();
}

void
RGSettings::setDeleteBMPs(bool val)
{
  QSettings settings;
  settings.setValue("bmp2aviDeleteBMPs", val);
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

void
RGSettings::setPenColor(const QColor &newCol)
{
  QSettings settings;
  settings.beginGroup("routeColor");
  settings.setValue("red", newCol.red());
  settings.setValue("green", newCol.green());
  settings.setValue("blue", newCol.blue());
  settings.endGroup();
}


int
RGSettings::getPenSize()
{
  QSettings settings;
  return settings.value("routeWidth", 5).toInt();
}

void
RGSettings::setPenSize(int size)
{
  QSettings settings;
  settings.setValue("routeStyle", size);
}

int
RGSettings::getPenStyle()
{
  QSettings settings;
  return settings.value("routeStyle", 1).toInt();
}

void
RGSettings::setPenStyle(int style)
{
  QSettings settings;
  settings.setValue("routeStyle", style);
}

QString
RGSettings::getLastOpenDir()
{
  QSettings settings;
  return settings.value("lastOpenDir", QDir::homePath()).toString();
}

void
RGSettings::setLastOpenDir(const QString &fileName)
{
  QSettings settings;
  settings.setValue("lastOpenDir", fileName);
}
QString
RGSettings::getLastSaveDir()
{
  QSettings settings;
  return settings.value("lastSaveDir", QDir::homePath()).toString();
}

void
RGSettings::setLastSaveDir(const QString &fileName)
{
  QSettings settings;
  settings.setValue("lastSaveDir", fileName);
}

QString
RGSettings::getLastGenDir()
{
  QSettings settings;
  return settings.value("lastGenDir", QDir::homePath()).toString();
}

void
RGSettings::setLastGenDir(const QString &dir)
{
  QSettings settings;
  settings.setValue("lastGenDir", dir);
}

int
RGSettings::getVehicleAngle(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleAngles");
  int angle = settings.value(name, 0).toInt();
  settings.endGroup();
  return angle;
}

void
RGSettings::setVehicleAngle(const QString &name, int angle)
{
  QSettings settings;
  settings.beginGroup("vehicleAngles");
  settings.setValue(name, angle);
  settings.endGroup();
}

bool
RGSettings::getVehicleMirrored(const QString &name)
{
  QSettings settings;
  settings.beginGroup("vehicleMirror");
  bool mirrored = settings.value(name, false).toBool();
  settings.endGroup();
  return mirrored;
}

void
RGSettings::setVehicleMirrored(const QString &name, bool mirror)
{
  QSettings settings;
  settings.beginGroup("vehicleMirror");
  settings.setValue(name, mirror);
  settings.endGroup();
}

void
RGSettings::setInterpolationMode(bool val)
{
  QSettings settings;
  settings.setValue("interPolationMode", val);
}

bool
RGSettings::getInterpolationMode()
{
  QSettings settings;
  return settings.value("interPolationMode", true).toBool();
}

void
RGSettings::setRoutePlayTime(int time)
{
  QSettings settings;
  settings.setValue("routePlayTime", time);
}

int
RGSettings::getRoutePlayTime()
{
  QSettings settings;
  return settings.value("routePlayTime", 5).toInt();
}

void
RGSettings::setGMXResolution(int xres)
{
  QSettings settings;
  settings.setValue("gmXResolution", xres);
}

int
RGSettings::getGMXResolution()
{
  QSettings settings;
  return settings.value("gmXResolution", 768).toInt();
}

void
RGSettings::setGMYResolution(int yres)
{
  QSettings settings;
  settings.setValue("gmYResolution", yres);
}

int
RGSettings::getGMYResolution()
{
  QSettings settings;
  return settings.value("gmYResolution", 576).toInt();
}

void
RGSettings::setCurvedInterpolation(bool enable)
{
  QSettings settings;
  settings.setValue("curvedInterPolation", enable);
}

bool
RGSettings::getCurvedInterpolation(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return false;
  else
    return settings.value("curvedInterPolation", false).toBool();
}

void
RGSettings::setCurveRadius(int val)
{
  QSettings settings;
  settings.setValue("curveRadius", val);
}

int
RGSettings::getCurveRadius(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return 10;
  else
    return settings.value("curveRadius",10).toInt();
}


void
RGSettings::setVehicleStepAngle(int angle)
{
  QSettings settings;
  settings.setValue("vehicleStepAngle", angle);
}

int
RGSettings::getVehicleStepAngle(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return 5;
  else
    return settings.value("vehicleStepAngle", 5).toInt();
}

void
RGSettings::setVehicleMinDistance(int dist)
{
  QSettings settings;
  settings.setValue("vehicleMinDistance", dist);
}

int
RGSettings::getVehicleMinDistance(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return 10;
  else
    return settings.value("vehicleMinDistance", 10).toInt();
}

void
RGSettings::setVehicleForceCounter(int ctr)
{
  QSettings settings;
  settings.setValue("vehicleForceCounter", ctr);
}

int
RGSettings::getVehicleForceCounter(bool defaultVal)
{
  QSettings settings;
  if (defaultVal)
    return 20;
  else
    return settings.value("vehicleForceCounter", 20).toInt();
}
