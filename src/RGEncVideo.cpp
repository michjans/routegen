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

RGEncVideo::RGEncVideo(QWidget *parent) :
  QWidget(parent),
	mExecName(QString()),
  mExists(false),
  mFps(0),
  mOutName(QString()),
  mKeyFrameRate(0),
  mCompress(QString()),
  mCompressDefault(QString())
{
  mUi.setupUi(this);
  // hide all specific part of the Ui
  mUi.bitRateLabel->setVisible(false);
  mUi.bitRateLabel2->setVisible(false);
  mUi.bitRateSB->setVisible(false);
  mUi.bmp2AviLocBrowsePB->setVisible(false);
  mUi.bmp2AviLocLabel->setVisible(false);
  mUi.bmp2AviLocLE->setVisible(false);
  qDebug()<<"RGEncVideo";

}

RGEncVideo::~RGEncVideo()
{
  qDebug()<<"~RGEncVideo";
}

bool RGEncVideo::initCodecExecutable()
{
  while (checkForCodecExecutable(mExecName)==false){
    //Bmp2avi not found, ask user for different directory
    if (QMessageBox::question (NULL, encoderName() + " not found",
                               "Could not find "+ encoderName() +", do you want to browse for it?",
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
      mExecName = QFileDialog::getOpenFileName(NULL,
                                               QString("Select the directory where " + encoderName() + " is located."),
                                               QDir::currentPath(),
                                               "Executables (*.exe)");
    } else {
      break;
    }
  }

  if ( checkForCodecExecutable(mExecName)==true )
	{
    mExists=initCodecs();
  }
  else
	{
    QMessageBox::warning (NULL, tr("No video encoder found"), encoderName() + tr(" has not been found, therefore video generation will be unavailable.") +
                          tr("\nYou can set ") + encoderName() + tr(" directory in the preferences"));
    mExecName=QString();
  }
  RGSettings::setVideoEncExec(mExecName);
  mUi.bmp2AviLocLE->setText(mExecName);

  mUi.bmp2AviLocBrowsePB->setVisible(true);
  mUi.bmp2AviLocLabel->setVisible(true);
  mUi.bmp2AviLocLE->setVisible(true);
  QObject::connect(mUi.bmp2AviLocBrowsePB, SIGNAL(clicked(bool)), this, SLOT(browseClicked()));

	return mExists;
}

bool RGEncVideo::exists() const
{
  return mExists;
}

void RGEncVideo::updateFromSettings()
{
  mFps=RGSettings::getFps();
  mUi.fpsSB->setValue(mFps);
  mOutName=RGSettings::getAviOutName();
  mUi.nameOutputLE->setText(mOutName);
  mKeyFrameRate=RGSettings::getKeyFrameRate();
  mUi.keyFrSB->setValue(mKeyFrameRate);
  mUi.deleteBMPsCB->setChecked(RGSettings::getDeleteBMPs());
  mCompress=RGSettings::getAviCompression();
  if(mCompress==QString())
    mCompress=mCompressDefault;
  int index=mUi.codecCB->findData(mCompress);
  if (index>=0)
    mUi.codecCB->setCurrentIndex(index);
}

void RGEncVideo::saveInSettings()
{
  mOutName=mUi.nameOutputLE->text();
  RGSettings::setAviOutName(mOutName);
  RGSettings::setDeleteBMPs(mUi.deleteBMPsCB->isChecked());
  mFps=mUi.fpsSB->value();
  RGSettings::setFps(mFps);
  mKeyFrameRate=mUi.keyFrSB->value();
  RGSettings::setKeyFrameRate(mKeyFrameRate);
  mCompress = mUi.codecCB->itemData(mUi.codecCB->currentIndex()).toString();
  RGSettings::setAviCompression(mCompress);

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
    emit movieGenerationFinished();
    return;
  }
  qDebug()<<"you shoudl'nt see that";
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
  delete mProcessWaitMessage;
  QMessageBox::critical (NULL, "Error", encoderName() + " execution failed!" );

  mVideoEncProcess->kill();

  if (mVideoEncProcess->state() != QProcess::NotRunning)
    QMessageBox::critical (NULL, "Error", QString("Unable to kill ") + encoderName() + ", check your processes!" );

  mVideoEncProcess->deleteLater();
  emit movieGenerationFinished();
}

void RGEncVideo::browseClicked()
{
  QString codecExecLoc = mUi.bmp2AviLocLE->text();
  codecExecLoc = QFileDialog::getOpenFileName(this, tr("Select ") + encoderName() + tr(" location"),
                                            codecExecLoc,
                                            tr("Bmp2Avi ( ") + encoderExecBaseName() + ")");
  if (codecExecLoc==QString()) return;//cancel pressed
  if (checkForCodecExecutable(codecExecLoc))
	{
    mExecName=codecExecLoc;
    mUi.bmp2AviLocLE->setText(mExecName);
    mExists=initCodecs();
  }
  else
	{
    mExists=false;
	}
  mExecName=QString();
  mUi.bmp2AviLocLE->setText(mExecName);
}

bool RGEncVideo::checkForCodecExecutable(const QString &execName)
{
  QFile codecExec(execName);
  if (codecExec.exists() == false) return false;
  if (!codecExec.fileName().contains(encoderName())) return false;
  return true;
}
