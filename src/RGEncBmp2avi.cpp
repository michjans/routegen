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

RGEncBmp2avi::RGEncBmp2avi(QWidget *parent) :
  RGEncVideo(parent)
{
  mCompressDefault=QString("DIB");
  qDebug()<<"Bmp2avi encoder class";
}

void RGEncBmp2avi::updateFromSettings()
{
  RGEncVideo::updateFromSettings();
  //For bmp2avi we don't support the custom commandline
  mUi.mCommandLineCB->setEnabled(false);
}

void RGEncBmp2avi::saveInSettings()
{
  RGEncVideo::saveInSettings();
}

void RGEncBmp2avi::generateMovie(const QString &dirName)
{
  QStringList arguments;
  arguments << "-f" << QString("%1").arg(mFps) << "-k" << QString("%1").arg(mKeyFrameRate) << "-o" << QString(mOutName) << "-c" << mCompress;

  this->createEncodingProcess(dirName,mExecName,arguments);
}

QString RGEncBmp2avi::encoderName()
{
  return QString("bmp2avi");
}

QString RGEncBmp2avi::encoderExecBaseName()
{
#ifdef Q_OS_WIN
  return QString("bmp2avi.exe");
#else
  return QString("bmp2avi");
#endif

}

bool RGEncBmp2avi::initCodecs()
{
  //Collect codecs from bmp2avi
  mUi.codecCB->clear();
  QProcess *codecExecProcess = new QProcess(this);
  QStringList arguments;
  //List codecs
  arguments << "-l";
  codecExecProcess->start(mExecName, arguments);
  if (codecExecProcess->waitForFinished()){
    int currentIndex = 0;
    //First add "Uncompressed" codec to combobox
    mUi.codecCB->addItem("Uncompressed", "DIB");
    QByteArray output = codecExecProcess->readAllStandardOutput();
    QList<QByteArray> lines = output.split('\n');
    bool inHeader = true;
    for (int i = 0; i < lines.size(); i++){
      QString line = lines[i].data();
      if (inHeader){
        //Skip header
        if (!line.startsWith("------")) continue;
        else inHeader = false;
      } else {
        if (!line.contains("|")) continue;
        //List of codecs starts
        QStringList codec = line.split('|');
        if (codec.size() == 2) {
          //qDebug() << "CODE: " << codec[0].trimmed();
          //qDebug() << "DESCR:" << codec[1].trimmed();
          //Add description to combobox and codec code as data
          mUi.codecCB->addItem(codec[1].trimmed(), codec[0].trimmed());
          if (codec[0].trimmed() == mCompress) currentIndex = mUi.codecCB->count() - 1;
        }
        //else: don't know what this would be...
      }
    }
    //Set current setting
    mUi.codecCB->setCurrentIndex(currentIndex);
  }
  else{
    QMessageBox::critical (this, "Error", "Could not run bmp2avi to collect codec selection!");
		return false;
  }

	return true;
}
