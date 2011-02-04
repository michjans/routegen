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

#include "RGEncVideo.h"
#include <QtGui>
#include <RGSettings.h>

RGEncVideo::RGEncVideo(QObject *parent) :
    QObject(parent),
    mExists(false),
    mFps(0),
    mOutName(QString()),
    mKeyFrameRate(0)
{
  qDebug()<<"RGEncVideo";
  mFps=RGSettings::getFps();
  mOutName=RGSettings::getAviOutName();
  mKeyFrameRate=RGSettings::getKeyFrameRate();
}

bool RGEncVideo::exists() const
{
  return mExists;
}

void RGEncVideo::fillSettingsUi(Ui::Dialog ui)
{
  ui.mBmp2AviOutNameLE->setText("TTTTT");
}

void RGEncVideo::getFromSettingsUi(Ui::Dialog ui)
{

}
void RGEncVideo::createEncodingProcess(const QString &dirName,const QString &videoEncExec,const QStringList &arguments)
{
  if (!mExists){
    QString txt = QString(
        "<html>"
        "<p>"
        "Your route has been generated in the selected directory. "
        "Each frame is generated as a *.bmp file in that directory. "
        "</p>"
        "<p><b>NOTE: Since no video encoder is available, no avi file is generated!</b></p>"
        "</html>"
        );

    QMessageBox::information (NULL, "Map Generation Finished", txt );
    return ;
  }

  mVideoEncProcess = new QProcess(this);
  QObject::connect(mVideoEncProcess, SIGNAL(finished (int , QProcess::ExitStatus)),
                   this, SLOT(encodingProcessFinished(int , QProcess::ExitStatus)));
  QObject::connect(mVideoEncProcess, SIGNAL(error (QProcess::ProcessError)),
                   this, SLOT(encodingProcessError(QProcess::ProcessError)));
  mVideoEncProcess->setWorkingDirectory(dirName);
  mVideoEncProcess->start(videoEncExec, arguments);

  mProcessWaitMessage = new QMessageBox(NULL);
  mProcessWaitMessage->setWindowTitle("One moment please...");
  mProcessWaitMessage->setText(QString("Executing ") + encoderName() + " to convert BMP files to video file, one moment please...");
  mProcessWaitMessage->setStandardButtons(QMessageBox::NoButton);
  mProcessWaitMessage->setCursor(Qt::WaitCursor);
  mProcessWaitMessage->show();
}

void RGEncVideo::encodingProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  delete mProcessWaitMessage;
  QByteArray output = mVideoEncProcess->readAllStandardOutput();
  QString logFileName(encoderName());
  logFileName.append(".log");

  QFile logFile(mVideoEncProcess->workingDirectory() + "/" + logFileName);
  if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    logFile.write(output);
    logFile.close();
  } else {
    QMessageBox::critical (NULL, "Error",
                           QString("Unable to write ") + logFileName + "! Disk full or no permissions?");
  }

  if (exitStatus == QProcess::NormalExit && exitCode == 0 && !output.contains("Error")) {

    QString txt = QString(
        "<html>"
        "<center>"
        "<p>"
        "Your route has been generated in the selected directory. "
        "Each frame is generated as a *.bmp file in that directory. "
        "The name of the generated movie is <b>") + mOutName + QString(".avi</b>."
                                                                      "</p>"
                                                                      "</center>"
                                                                      "</html>"
                                                                      );
    QMessageBox::information (NULL, "Map Generation Finished", txt );

  } else {
    QMessageBox::critical (NULL, "Error", encoderName() + " did not finish successfully! See file "+ logFileName +" in output directory for details.");
  }

  mVideoEncProcess->deleteLater();
  emit movieGenerationFinished();
}

void RGEncVideo::encodingProcessError(QProcess::ProcessError)
{
  QMessageBox::critical (NULL, "Error", encoderName() + " execution failed!" );

  mVideoEncProcess->kill();

  if (mVideoEncProcess->state() != QProcess::NotRunning)
    QMessageBox::critical (NULL, "Error", QString("Unable to kill ") + encoderName() + ", check your processes!" );

  mVideoEncProcess->deleteLater();
  emit movieGenerationFinished();
}
