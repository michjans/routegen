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

#include "RGEncBmp2avi.h"
#include <QtGui>
#include <RGSettings.h>

RGEncBmp2avi::RGEncBmp2avi(QObject *parent) :
    RGEncVideo(parent),
    mExecName(QString()),
    mCompress(QString())
{
  qDebug()<<"Bmp2avi encoder class";
  //Currently we only initialize bmp2avi
  mExecName = RGSettings::getVideoEncExec();
  mCompress=RGSettings::getAviCompression();
  QFile bmp2aviExec(mExecName);
  while (bmp2aviExec.exists() == false || !bmp2aviExec.fileName().contains(QString("bmp2avi"))){
    //Bmp2avi not found, ask user for different directory
    if (QMessageBox::question (NULL, "Bmp2Avi not found",
                               "Could not find bmp2avi.exe, do you want to browse for it?",
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
      mExecName = QFileDialog::getOpenFileName(NULL,
                                                     QString("Select the directory where bmp2avi.exe is located."),
                                                     QDir::currentPath(),
                                                     "Executables (*.exe)");
      bmp2aviExec.setFileName(mExecName);
    } else {
      break;
    }
  }

  if (bmp2aviExec.exists()){
    //Store new location
    RGSettings::setVideoEncExec(mExecName);
    mExists=true;
  }
  else
    QMessageBox::warning (NULL, "Not available", "Avi generation using bmp2avi will be unavailable");
}

void RGEncBmp2avi::generateMovie(const QString &dirName, const QString &filePrefix)
{
  Q_UNUSED(filePrefix);
  QStringList arguments;
  arguments << "-f" << QString("%1").arg(mFps) << "-k" << QString("%1").arg(mKeyFrameRate) << "-o" << QString(mOutName).append(".avi") << "-c" << mCompress;

  this->createEncodingProcess(dirName,mExecName,arguments);
}

QString RGEncBmp2avi::encoderName()
{
  return QString("Bmp2avi");
}
