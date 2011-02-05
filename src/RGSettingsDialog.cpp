/*
    Copyright (C) 2008-2011  Michiel Jansen

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
#include <QDebug>

#include "RGSettingsDialog.h"
#include "RGSettings.h"

RGSettingsDialog::RGSettingsDialog(QWidget *videoSettings,QWidget *parent)
  :QDialog(parent)
{
  ui.setupUi(this);

  mGenerateBeginEndFramesCB = ui.mIconLessBeginEndFramesCB;
  mSmoothLengthSB = ui.mSmoothLengthSB;
  mResetDefaultsPB = ui.mResetDefaultsPB;

  //Advanced tab
  mSmoothLengthSB->setValue(RGSettings::getSmoothLength());
  mGenerateBeginEndFramesCB->setChecked(!RGSettings::getIconLessBeginEndFrames());

  ui.tabWidget->insertTab(0,videoSettings,QString("Movie Generation"));
  ui.tabWidget->setCurrentIndex(0);
}

RGSettingsDialog::~RGSettingsDialog()
{
  qDebug()<<"~RGSettingsDialog";
  ui.tabWidget->widget(0)->setParent(NULL);
}

void RGSettingsDialog::on_mResetDefaultsPB_clicked(bool)
{
  mSmoothLengthSB->setValue(RGSettings::getSmoothLength(true));
}

void RGSettingsDialog::accept()
{
  //Advanced settings
  RGSettings::setSmoothLength(mSmoothLengthSB->value());
  RGSettings::setIconLessBeginEndFrames(!mGenerateBeginEndFramesCB->isChecked());
  QDialog::accept();
}
