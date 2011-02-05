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

#ifndef RGENCVIDEO_H
#define RGENCVIDEO_H

#include <QObject>
#include <QProcess>
#include <QMessageBox>
#include "ui_videosettings.h"

class RGEncVideo : public QWidget
{
  Q_OBJECT
public:
  explicit RGEncVideo(QWidget *parent = 0);
  ~RGEncVideo();
  bool exists() const;
  virtual void updateFromSettings();
  virtual void saveInSettings();
  virtual void generateMovie(const QString &dirName, const QString &filePrefix)=0;
  virtual QString encoderName()=0;

signals:
  void movieGenerationFinished();

protected slots:
  void createEncodingProcess(const QString &dirName,const QString &videoEncExec,const QStringList &arguments);
  void encodingProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void encodingProcessError(QProcess::ProcessError error);

protected:
  bool mExists;
  int mFps;
  QString mOutName;
  int mKeyFrameRate;
  QMessageBox    *mProcessWaitMessage;
  QProcess    *mVideoEncProcess;
  QString mCompress;
  QString mCompressDefault;
  Ui::videoSettings mUi;

};

#endif // RGENCVIDEO_H
