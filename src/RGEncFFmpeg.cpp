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

RGEncFFmpeg::RGEncFFmpeg(QWidget* parent)
    : RGEncVideo(parent),
      mBitRate(100)
{
    mCompressDefault = QString("h264");
    qDebug() << "FFMpeg encoder class";

    mUi.bitRateLabel->setVisible(true);
    mUi.bitRateLabel2->setVisible(true);
    mUi.bitRateSB->setVisible(true);

    mUi.frameFileCB->addItem(QString("bmp"));
    mUi.frameFileCB->addItem(QString("jpg"));
    mUi.frameFileCB->addItem(QString("png"));
    mUi.frameFileCB->addItem(QString("tif"));

    mUi.outputFileCB->addItem(QString("avi"));
    mUi.outputFileCB->addItem(QString("mov"));
    mUi.outputFileCB->addItem(QString("mp4"));
    mUi.outputFileCB->addItem(QString("mpg"));
    mUi.outputFileCB->addItem(QString("mkv"));
    mUi.outputFileCB->addItem(QString("wmv"));

    QObject::connect(mUi.mCommandLineCB, &QAbstractButton::toggled, this, &RGEncFFmpeg::handleManualCommandLineChecked);
}

void RGEncFFmpeg::updateFromSettings()
{
    RGEncVideo::updateFromSettings();
    mBitRate = RGSettings::getBitRate();
    mUi.bitRateSB->setValue(mBitRate);
    mUi.mCommandLineCB->setChecked(RGSettings::getManualCommandLineChecked());

    QString frameFileType = RGSettings::getFrameFileType();
    if (frameFileType.isEmpty())
    {
        frameFileType = "bmp";
    }
    mUi.frameFileCB->setCurrentText(frameFileType);

    QString outputFileType = RGSettings::getFFMpegOutputFileType();
    if (outputFileType.isEmpty())
    {
        outputFileType = "mp4";
    }
    mUi.outputFileCB->setCurrentText(outputFileType);

    if (RGSettings::getManualCommandLineChecked())
    {
        mUi.mCommandLineLE->setText(RGSettings::getFFMpegCommandlineArgs());
    }
}

void RGEncFFmpeg::saveInSettings()
{
    RGEncVideo::saveInSettings();
    mBitRate = mUi.bitRateSB->value();
    RGSettings::setBitRate(mBitRate);
    RGSettings::setManualCommandLineChecked(mUi.mCommandLineCB->isChecked());
    if (mUi.mCommandLineCB->isChecked())
    {
        RGSettings::setFFMpegCommandlineArgs(mUi.mCommandLineLE->text());
    }

    RGSettings::setFrameFileType(mUi.frameFileCB->currentText());
    RGSettings::setFFMpegOutputFileType(mUi.outputFileCB->currentText());
}

void RGEncFFmpeg::generateMovie(const QString& dirName)
{
    QStringList arguments;
    if (RGSettings::getManualCommandLineChecked())
    {
        arguments = RGSettings::getFFMpegCommandlineArgs().split(" ");
        qDebug() << "ffmpeg arguments in:" << RGSettings::getFFMpegCommandlineArgs();
        qDebug() << "ffmpeg arguments out:" << arguments;
    }
    else
    {
        arguments << "-report"
                  << "-y"
                  << "-framerate" << QString("%1").arg(mFps) << "-i" << QString("map").append("%05d.").append(frameFileType()) << "-g"
                  << QString("%1").arg(mKeyFrameRate) << "-r" << QString("%1").arg(mFps) << "-b" << QString("%1k").arg(mBitRate) << "-vcodec" << mCompress
                  << QString("-pix_fmt") << QString("yuv420p") << QString(mOutName).append(".").append(outputFileType());
    }

    this->createEncodingProcess(dirName, mExecName, arguments);
}

QString RGEncFFmpeg::encoderName()
{
    return QString("ffmpeg");
}

QString RGEncFFmpeg::encoderExecBaseName()
{
#ifdef Q_OS_WIN
    return QString("ffmpeg.exe");
#else
    return QString("ffmpeg");
#endif
}

QString RGEncFFmpeg::frameFileType() const
{
    return RGSettings::getFrameFileType();
}

QString RGEncFFmpeg::outputFileType() const
{
    return RGSettings::getFFMpegOutputFileType();
}

bool RGEncFFmpeg::initCodecs()
{
    //Check if FFmpeg is present
    QProcess checkFFmpeg;
    checkFFmpeg.setProcessChannelMode(QProcess::MergedChannels);

    QStringList arguments;
    arguments << "-version";
    checkFFmpeg.start(mExecName, arguments);
    if (!checkFFmpeg.waitForFinished())
    {
        QMessageBox::warning(nullptr, "No video encoder",
                             "FFmpeg has not been found, video generation will be unavailable.\nPlease install FFmpeg and restart the application.");
        return false;
    }
    else
    {
        mExists = true;
    }

    QProcess* ffmpegProcess = new QProcess(this);
    arguments.clear();
    //List codecs
    arguments << "-codecs";
    //ffmpegProcess = new QProcess(this);
    ffmpegProcess->start(mExecName, arguments);
    if (ffmpegProcess->waitForFinished())
    {
        int currentIndex = 0;
        QByteArray output = ffmpegProcess->readAllStandardOutput();
        QList<QByteArray> lines = output.split('\n');
        bool inHeader = true;
        qDebug() << "lines" << lines.size();
        for (int i = 0; i < lines.size(); i++)
        {
            QString line = lines[i].data();

            if (inHeader)
            {
                //Skip header
                if (!line.startsWith(" ------"))
                    continue;
                else
                    inHeader = false;
            }
            else
            {
                if (line.size() < 10)
                    continue;
                //select only video encoder codec
                if (line.at(2) != QString("E").at(0))
                    continue;
                if (line.at(3) != QString("V").at(0))
                    continue;
                //List of codecs starts
                line.remove(0, 8);
                qDebug() << line;
                QString codec, codecDesc;
                codec = line.left(line.indexOf(" ")).trimmed();
                codecDesc = (line.remove(0, (line.indexOf(" "))).trimmed());
                if (codec == mCompressDefault)
                    codecDesc.append("<----Default codec for Routegen");
                mUi.codecCB->addItem(codecDesc, codec);
                if (codec == mCompress)
                    currentIndex = mUi.codecCB->count() - 1;
            }
        }
        //Set current setting
        mUi.codecCB->setCurrentIndex(currentIndex);
    }
    else
    {
        QMessageBox::critical(this, "Error", "Could not run ffmpeg to collect codec selection!");
        return false;
    }

    return true;
}

void RGEncFFmpeg::handleManualCommandLineChecked(bool checked)
{
    if (checked)
    {
        QString keyFrameRate = QString::number(mUi.keyFrSB->value());
        QString fps = QString::number(mUi.fpsSB->value());
        QString bitRate = QString::number(mUi.bitRateSB->value());
        QString compress = mUi.codecCB->itemData(mUi.codecCB->currentIndex()).toString();
        QString outName = mUi.nameOutputLE->text();

        QString arguments = "-report -y -framerate " + QString("%1").arg(fps) + " -i ";
        arguments += QString("map").append("%05d.").append(frameFileType()) + " -g" + QString(" %1").arg(keyFrameRate) + " -r" + QString(" %1").arg(fps) +
                     " -b" + QString(" %1k").arg(bitRate) + " -vcodec " + compress + " -pix_fmt yuv420p " +
                     QString(outName).append(".").append(outputFileType());
        mUi.mCommandLineLE->setText(arguments);
    }
    else
    {
        mUi.mCommandLineLE->clear();
    }
}
