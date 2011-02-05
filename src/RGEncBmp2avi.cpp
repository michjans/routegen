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

  mUi.bmp2AviLocBrowsePB->setVisible(true);
  mUi.bmp2AviLocLabel->setVisible(true);
  mUi.bmp2AviLocLE->setVisible(true);
  QObject::connect(mUi.bmp2AviLocBrowsePB, SIGNAL(clicked(bool)), this, SLOT(browseClicked()));
  updateFromSettings();
}

void RGEncBmp2avi::updateFromSettings()
{
  RGEncVideo::updateFromSettings();
  mExecName=RGSettings::getVideoEncExec();
  mUi.bmp2AviLocLE->setText(mExecName);
}

void RGEncBmp2avi::saveInSettings()
{
  RGEncVideo::saveInSettings();
  mExecName=mUi.bmp2AviLocLE->text();
  RGSettings::setVideoEncExec(mExecName);
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

void RGEncBmp2avi::browseClicked()
{
  QString bmp2AviLoc = mUi.bmp2AviLocLE->text();
  bmp2AviLoc = QFileDialog::getOpenFileName(this, tr("Select BMP2AVI location"),
                                            bmp2AviLoc,
                                            tr("Bmp2Avi (bmp2avi.exe)"));
  if (!bmp2AviLoc.isNull()){
    mUi.bmp2AviLocLE->setText(bmp2AviLoc);
  }
}

//Collect codecs from bmp2avi
/*QString bmp2aviExecName = RGSettings::getVideoEncExec();
QFile bmp2aviExec(bmp2aviExecName);
qDebug() << bmp2aviExecName;
if (bmp2aviExec.exists()) {
  qDebug() << "-->exists";
  QProcess *bmp2AviProcess = new QProcess(this);
  QStringList arguments;

  //List codecs
  arguments << "-l";

  bmp2AviProcess = new QProcess(this);
  bmp2AviProcess->start(bmp2aviExecName, arguments);
  if (bmp2AviProcess->waitForFinished()){
    QString currentCodec = RGSettings::getAviCompression();
    int currentIndex = 0;
    //First add "Uncompressed" codec to combobox
    mCodecCB->addItem("Uncompressed", "DIB");
    QByteArray output = bmp2AviProcess->readAllStandardOutput();
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
          mCodecCB->addItem(codec[1].trimmed(), codec[0].trimmed());
          if (codec[0].trimmed() == currentCodec) currentIndex = mCodecCB->count() - 1;
        }
        //else: don't know what this would be...
      }
    }
    //Set current setting
    mCodecCB->setCurrentIndex(currentIndex);
  }
  else{
    QMessageBox::critical (this, "Error", "Could not run bmp2avi to collect codec selection!");
  }

}

*/
