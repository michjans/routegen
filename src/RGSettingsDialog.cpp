/*
    Copyright (C) 2008-2009  Michiel Jansen

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

#include "RGSettingsDialog.h"
#include "RGSettings.h"
#include "ui_settings.h"

RGSettingsDialog::RGSettingsDialog(QWidget *parent)
  :QDialog(parent)
{
  Ui::Dialog ui;
  ui.setupUi(this);

  mBmp2AviLocLE = ui.mBmp2AviLocLE;
  mBrowsePB = ui.mBrowsePB;
  mBmp2AviOutNameLE = ui.mBmp2AviOutNameLE;
  mDeleteBMPsCB = ui.mDeleteBMPsCB;
  mFpsSB = ui.mFpsSB;
  mKeyFrSB = ui.mKeyFrSB;
  mCodecCB = ui.mCodecCB;
  mSmoothCurvesModeCB = ui.mSmoothCurvesModeCB;
  mCurveRadiusSB = ui.mCurveRadiusSB;
  mResetDefaultsPB = ui.mResetDefaultsPB;

  QObject::connect(mBrowsePB, SIGNAL(clicked(bool)), this, SLOT(browseClicked()));
  QObject::connect(mSmoothCurvesModeCB, SIGNAL(toggled(bool)), mCurveRadiusSB, SLOT(setEnabled(bool)));

  //Deactivate Path to bmp2avi.exe on linux :
  #ifdef Q_WS_X11
    mBmp2AviLocLE->setDisabled(true);
    mBrowsePB->setDisabled(true);
  #endif

  initFromSettings();
}

void RGSettingsDialog::on_mResetDefaultsPB_clicked(bool)
{
  mSmoothCurvesModeCB->setChecked(RGSettings::getCurvedInterpolation(true));
  mCurveRadiusSB->setValue(RGSettings::getCurveRadius(true));
}

void RGSettingsDialog::browseClicked()
{
  QString bmp2AviLoc = mBmp2AviLocLE->text();
  bmp2AviLoc = QFileDialog::getOpenFileName(this, tr("Select BMP2AVI location"),
                                                 bmp2AviLoc,
                                                 tr("Bmp2Avi (bmp2avi.exe)")); 
  if (!bmp2AviLoc.isNull())
  {
    mBmp2AviLocLE->setText(bmp2AviLoc);
    RGSettings::setVideoEncoder(QString("bmp2avi"));
  }
}

void RGSettingsDialog::accept()
{
  RGSettings::setVideoEncExec(mBmp2AviLocLE->text());
  RGSettings::setAviOutName(mBmp2AviOutNameLE->text());
  RGSettings::setDeleteBMPs(mDeleteBMPsCB->isChecked());
  RGSettings::setFps(mFpsSB->value());
  RGSettings::setKeyFrameRate(mKeyFrSB->value());
  QVariant codec = mCodecCB->itemData(mCodecCB->currentIndex());
  RGSettings::setAviCompression(codec.toString());

  //Advanced settings
  RGSettings::setCurvedInterpolation(mSmoothCurvesModeCB->isChecked());
  RGSettings::setCurveRadius(mCurveRadiusSB->value());

  QDialog::accept();
}

void RGSettingsDialog::initFromSettings()
{
  mFpsSB->setRange(0,50);
  mKeyFrSB->setRange(0,50);

  mBmp2AviLocLE->setText(RGSettings::getVideoEncExec());
  mBmp2AviOutNameLE->setText(RGSettings::getAviOutName());
  mDeleteBMPsCB->setChecked(RGSettings::getDeleteBMPs());
  mFpsSB->setValue(RGSettings::getFps());
  mKeyFrSB->setValue(RGSettings::getKeyFrameRate());

  //Advanced tab
  mSmoothCurvesModeCB->setChecked(RGSettings::getCurvedInterpolation());
  mCurveRadiusSB->setValue(RGSettings::getCurveRadius());
  mCurveRadiusSB->setRange(1, 1000);
  mCurveRadiusSB->setEnabled(RGSettings::getCurvedInterpolation());

  //Collect codecs from bmp2avi
  QString bmp2aviExecName = RGSettings::getVideoEncExec();
  QFile bmp2aviExec(bmp2aviExecName);
  if (bmp2aviExec.exists()) {
    QProcess *bmp2AviProcess = new QProcess(this);
    QStringList arguments;
   
    //List codecs
    arguments << "-l";

    bmp2AviProcess = new QProcess(this);
    bmp2AviProcess->start(bmp2aviExecName, arguments);
    if (bmp2AviProcess->waitForFinished())
    {
      QString currentCodec = RGSettings::getAviCompression();
      int currentIndex = 0;
      //First add "Uncompressed" codec to combobox
      mCodecCB->addItem("Uncompressed", "DIB");
      QByteArray output = bmp2AviProcess->readAllStandardOutput();
      QList<QByteArray> lines = output.split('\n');
      bool inHeader = true;
      for (int i = 0; i < lines.size(); i++)
      {
        QString line = lines[i].data();
        if (inHeader) {
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
    else
    {
      QMessageBox::critical (this, "Error", "Could not run bmp2avi to collect codec selection!");
    }

  }
}

//void RGSettingsDialog::on_mCodecCB_activated(int index)
//{
//  QVariant data = mCodecCB->itemData(index);
//  qDebug() << "CODECCHANGE:" << data.toString();
//}
