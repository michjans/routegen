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
#include <QFileDialog>
#include <QStandardPaths>
#include <QtGui>

#include <RGSettings.h>

RGEncVideo::RGEncVideo(QWidget* parent)
    : QWidget(parent),
      mExecName(QString()),
      mExists(false),
      mFps(0),
      mOutName(QString()),
      mKeyFrameRate(0),
      mProcessWaitMessage(0),
      mCompress(QString()),
      mCompressDefault(QString())
{
    mUi.setupUi(this);

    // hide all specific part of the Ui
    mUi.bitRateLabel->setVisible(false);
    mUi.bitRateLabel2->setVisible(false);
    mUi.bitRateSB->setVisible(false);
    mUi.codecExecLocBrowsePB->setVisible(false);
    mUi.codecExecLocLabel->setVisible(false);
    mUi.codecExecLocLE->setVisible(false);
    qDebug() << "RGEncVideo";
}

RGEncVideo::~RGEncVideo()
{
    qDebug() << "~RGEncVideo";
}

bool RGEncVideo::initCodecExecutable()
{
    mExecName = RGSettings::getVideoEncExec();
#ifdef Q_OS_WIN
    if (checkForCodecExecutable(mExecName) == false)
    {
        RGSettings::setAviCompression(QString());
        mExecName = QFileDialog::getOpenFileName(nullptr, tr("Select the directory where %1 is located.").arg(encoderName()), QDir::currentPath(),
                                                 tr("Executables (*.exe)"));
    }
#endif

    if (checkForCodecExecutable(mExecName) == true)
    {
        mExists = initCodecs();
    }
    else
    {
        QMessageBox::warning(nullptr, tr("No video encoder found"),
                             tr("%1 has not been found, therefore video generation will be unavailable.\nYou can set the %2 directory in the preferences")
                                 .arg(encoderName(), encoderName()));
        mExecName = QString();
    }
    RGSettings::setVideoEncExec(mExecName);
    updateFromSettings();
#ifdef Q_OS_WIN
    mUi.codecExecLocLE->setText(mExecName);
    mUi.codecExecLocLabel->setText(encoderName());
    mUi.codecExecLocBrowsePB->setVisible(true);
    mUi.codecExecLocLabel->setVisible(true);
    mUi.codecExecLocLE->setVisible(true);
    QObject::connect(mUi.codecExecLocBrowsePB, SIGNAL(clicked(bool)), this, SLOT(browseClicked()));
#endif

    return mExists;
}

bool RGEncVideo::exists() const
{
    return mExists;
}

void RGEncVideo::updateFromSettings()
{
    mFps = RGSettings::getFps();
    mUi.fpsSB->setValue(mFps);
    mOutName = RGSettings::getAviOutName();
    mUi.nameOutputLE->setText(mOutName);
    mKeyFrameRate = RGSettings::getKeyFrameRate();
    mUi.keyFrSB->setValue(mKeyFrameRate);
    mUi.deleteBMPsCB->setChecked(RGSettings::getDeleteBMPs());
    mUi.mBeginDelaySB->setValue(RGSettings::getBeginDelaySeconds());
    mUi.mEndDelaySB->setValue(RGSettings::getEndDelaySeconds());
    mCompress = RGSettings::getAviCompression();
    if (mCompress == QString())
        mCompress = mCompressDefault;
    int index = mUi.codecCB->findData(mCompress);
    if (index >= 0)
        mUi.codecCB->setCurrentIndex(index);
    mExecName = RGSettings::getVideoEncExec();
    mUi.codecExecLocLE->setText(mExecName);
}

void RGEncVideo::saveInSettings()
{
    mOutName = mUi.nameOutputLE->text();
    RGSettings::setAviOutName(mOutName);
    RGSettings::setDeleteBMPs(mUi.deleteBMPsCB->isChecked());
    RGSettings::setBeginDelaySeconds(mUi.mBeginDelaySB->value());
    RGSettings::setEndDelaySeconds(mUi.mEndDelaySB->value());
    mFps = mUi.fpsSB->value();
    RGSettings::setFps(mFps);
    mKeyFrameRate = mUi.keyFrSB->value();
    RGSettings::setKeyFrameRate(mKeyFrameRate);
    mCompress = mUi.codecCB->itemData(mUi.codecCB->currentIndex()).toString();
    RGSettings::setAviCompression(mCompress);
    mExecName = mUi.codecExecLocLE->text();
    mExists = checkForCodecExecutable(mExecName);
    RGSettings::setVideoEncExec(mExecName);
}

void RGEncVideo::createEncodingProcess(const QString& dirName, const QString& videoEncExec, const QStringList& arguments)
{
    if (!mExists)
    {
        QString txt = tr("<html>"
                         "<p>"
                         "Your route has been generated in the selected directory. "
                         "Each frame is generated as a *.%1 file in that directory. "
                         "</p>"
                         "<p><b>NOTE: Since no video encoder is available, no video file is generated!</b></p>"
                         "</html>")
                          .arg(frameFileType());

        QMessageBox::information(nullptr, tr("Map Generation Finished"), txt);
        emit movieGenerationFinished();
        return;
    }
    mVideoEncProcess = new QProcess(this);
    QObject::connect(mVideoEncProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(encodingProcessFinished(int, QProcess::ExitStatus)));
    QObject::connect(mVideoEncProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(encodingProcessError(QProcess::ProcessError)));
    mVideoEncProcess->setWorkingDirectory(dirName);
    mVideoEncProcess->start(videoEncExec, arguments);

    mProcessWaitMessage = new QMessageBox(nullptr);
    mProcessWaitMessage->setWindowTitle(tr("One moment please..."));
    mProcessWaitMessage->setText(tr("Executing %1 to convert image files to video file, one moment please...").arg(encoderName()));
    mProcessWaitMessage->setStandardButtons(QMessageBox::NoButton);
    mProcessWaitMessage->setCursor(Qt::WaitCursor);
    mProcessWaitMessage->show();
}

void RGEncVideo::encodingProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (mProcessWaitMessage != 0)
    {
        delete mProcessWaitMessage;
    }
    QByteArray output = mVideoEncProcess->readAllStandardOutput();
    QString logFileName(encoderName());
    logFileName.append(".log");

    QFile logFile(mVideoEncProcess->workingDirectory() + "/" + logFileName);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        logFile.write(output);
        logFile.close();
    }
    else
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to write") + " " + logFileName + "! " + tr("Disk full or no permissions?"));
    }

    if (exitStatus == QProcess::NormalExit && exitCode == 0 && !output.contains("Error"))
    {

        QString txt = tr("<html>"
                         "<center>"
                         "<p>"
                         "Your route has been generated in the selected directory. "
                         "Each frame is generated as a *.%1 file in that directory. "
                         "The name of the generated movie is <b>%2.%3</b>."
                         "</p>"
                         "</center>"
                         "</html>")
                          .arg(frameFileType(), mOutName, outputFileType());
        QMessageBox::information(nullptr, tr("Map Generation Finished"), txt);
    }
    else
    {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("%1 did not finish successfully! See file %2 in output directory for details.").arg(encoderName(), logFileName));
    }

    mVideoEncProcess->deleteLater();
    emit movieGenerationFinished();
}

void RGEncVideo::encodingProcessError(QProcess::ProcessError)
{
    if (mProcessWaitMessage != 0)
    {
        delete mProcessWaitMessage;
    }
    QMessageBox::critical(nullptr, tr("Error"), tr("%1 execution failed!").arg(encoderName()));

    mVideoEncProcess->kill();

    if (mVideoEncProcess->state() != QProcess::NotRunning)
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to kill %1, check your processes!").arg(encoderName()));

    mVideoEncProcess->deleteLater();
    emit movieGenerationFinished();
}

void RGEncVideo::browseClicked()
{
    QString codecExecLoc = mUi.codecExecLocLE->text();
    codecExecLoc =
        QFileDialog::getOpenFileName(this, tr("Select %1 location").arg(encoderName()), codecExecLoc, encoderName() + " (" + encoderExecBaseName() + ")");
    if (codecExecLoc == QString())
        return; //cancel pressed
    if (checkForCodecExecutable(codecExecLoc))
    {
        mExecName = codecExecLoc;
        mUi.codecExecLocLE->setText(mExecName);
        mExists = initCodecs();
    }
    else
    {
        mExists = false;
    }
    mUi.codecExecLocLE->setText(mExecName);
}

bool RGEncVideo::checkForCodecExecutable(QString& execName)
{
#ifdef Q_OS_WIN
    QFile codecExec(execName);
    if (!codecExec.exists())
    {
        //First check the default location again
        execName = QDir::currentPath() + "/ffmpeg/bin/ffmpeg.exe";
        codecExec.setFileName(execName);
        if (!codecExec.exists())
        {
            //Still not found!
            return false;
        }
    }
    if (!codecExec.fileName().contains(encoderName()))
    {
        return false;
    }
#else
    //On other OS it should be found in the PATH
    return !QStandardPaths::findExecutable(execName).isEmpty();
#endif
    return true;
}
