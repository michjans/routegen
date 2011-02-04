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

#include "RGEncFFmpeg.h"
#include <QtGui>
#include <RGSettings.h>

RGEncFFmpeg::RGEncFFmpeg(QObject *parent) :
    RGEncVideo(parent),
    mBitRate(QString())
{
  mBitRate = RGSettings::getBitRate();
  qDebug()<<"FFMpeg encoder class";
  //Check if FFmpeg is present
  QProcess checkFFmpeg;
  checkFFmpeg.setProcessChannelMode(QProcess::MergedChannels);
  checkFFmpeg.start("ffmpeg -version");
  if (!checkFFmpeg.waitForFinished())
    QMessageBox::warning (NULL, "No video encoder", "FFmpeg has not been found, video generation will be unavailable.\nPlease install FFmpeg and restart the application.");
  else
    mExists=true;
}

void RGEncFFmpeg::generateMovie(const QString &dirName, const QString &filePrefix)
{
  QStringList arguments;
  arguments << "-y" << "-i" << QString(filePrefix).append("\%05d.bmp") << "-g" << QString("%1").arg(mKeyFrameRate) <<"-r"<<QString("%1").arg(mFps)<< "-b" <<mBitRate << QString(mOutName).append(".avi");

  this->createEncodingProcess(dirName,"ffmpeg",arguments);
}

QString RGEncFFmpeg::encoderName()
{
  return QString("FFmpeg");
}
