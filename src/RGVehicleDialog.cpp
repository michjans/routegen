/*
    Copyright (C) 2008-2011  Michiel Jansen
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

#include <QDebug>
#include <QIcon>
#include <QPainter>

#include "RGVehicleDialog.h"

RGVehicleDialog::RGVehicleDialog(QWidget *parent,RGVehicleList *vehicleList)
  : QDialog(parent),
  mVehicleList(vehicleList),
  mPlayTimer(NULL),
  mTimerCounter(0)
{
  mCurrentVehicleId=mVehicleList->getCurrentVehicleId();
  mLastVehicleId=mCurrentVehicleId;
  mLastVehicleSize=mVehicleList->getVehicle(mCurrentVehicleId)->getSize();
  mLastVehicleMirror=mVehicleList->getVehicle(mCurrentVehicleId)->getMirror();
  mLastVehicleStartAngle=mVehicleList->getVehicle(mCurrentVehicleId)->getStartAngle();
  ui.setupUi(this);
  for(int i=0;i<vehicleList->count();i++){
    QListWidgetItem *item = new QListWidgetItem(QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(40)),mVehicleList->getVehicle(i)->getName());
    ui.vehicleListWidget->addItem(item);
  }

  if (mPlayTimer == NULL) {
    mPlayTimer = new QTimer(this);
    QObject::connect(mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerEvent()));
  }

  ui.vehicleListWidget->setCurrentRow(mCurrentVehicleId);

  //arrow:
  QPixmap arrow(100,15);
  arrow.fill(Qt::transparent);
  QPainter painter(&arrow);
  QPen pen(Qt::DashLine);
  pen.setColor(Qt::red);
  painter.setPen(pen);
  painter.drawLine(0, 7, 99, 7);
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);
  painter.drawLine(99,7,95,3);
  painter.drawLine(99,7,95,11);
  ui.arrowPix->setPixmap(arrow);
}

RGVehicleDialog::~RGVehicleDialog()
{
  //qDebug() << "RGVehicleDialog::~RGVehicleDialog";
}

void RGVehicleDialog::setPen(const QPen &pen)
{
  mPen=pen;
  updateVehiclePreview();
}

void RGVehicleDialog::accept()
{
  mCurrentVehicleId = ui.vehicleListWidget->currentRow();
  mVehicleList->setCurrentVehicleId(mCurrentVehicleId);
  QDialog::accept();
}

void RGVehicleDialog::reject()
{
  mVehicleList->getVehicle(mLastVehicleId)->setSize(mLastVehicleSize);
  mVehicleList->getVehicle(mLastVehicleId)->setMirror(mLastVehicleMirror);
  mVehicleList->getVehicle(mLastVehicleId)->setStartAngle(mLastVehicleStartAngle);
  mVehicleList->setCurrentVehicleId(mLastVehicleId);
  QDialog::reject();
}

void RGVehicleDialog::on_vehicleListWidget_currentRowChanged(int currentRow)
{
  if(mPlayTimer->isActive())
    mPlayTimer->stop();
  mTimerCounter=0;
  mCurrentVehicleId = currentRow;
  ui.sizeSB->setValue(mVehicleList->getVehicle(mCurrentVehicleId)->getSize());
  ui.angleSB->setValue(mVehicleList->getVehicle(mCurrentVehicleId)->getStartAngle());
  ui.mirrorCB->setChecked(mVehicleList->getVehicle(mCurrentVehicleId)->getMirror());
  updateVehiclePreview();
  if (mVehicleList->getVehicle(mCurrentVehicleId)->getDelay()>0){
    mPlayTimer->setInterval(mVehicleList->getVehicle(mCurrentVehicleId)->getDelay());
    mPlayTimer->start();
  }
}

void RGVehicleDialog::on_sizeSB_valueChanged(int size)
{
  mVehicleList->getVehicle(mCurrentVehicleId)->setSize(size);
  updateVehiclePreview();
}

void RGVehicleDialog::on_angleSB_valueChanged(int angle)
{
  mVehicleList->getVehicle(mCurrentVehicleId)->setStartAngle(angle);
  updateVehiclePreview();
}

void RGVehicleDialog::on_resetSizePB_clicked(bool)
{
  mVehicleList->getVehicle(mCurrentVehicleId)->setSize(mVehicleList->getVehicle(mCurrentVehicleId)->getRawSize());
  ui.sizeSB->setValue(mVehicleList->getVehicle(mCurrentVehicleId)->getSize());
  updateVehiclePreview();
}

void RGVehicleDialog::on_resetAnglePB_clicked(bool)
{
  mVehicleList->getVehicle(mCurrentVehicleId)->setStartAngle(0);
  ui.angleSB->setValue(mVehicleList->getVehicle(mCurrentVehicleId)->getStartAngle());
  updateVehiclePreview();
}

void RGVehicleDialog::on_mirrorCB_toggled(bool state)
{
  mVehicleList->getVehicle(mCurrentVehicleId)->setMirror(state);
  updateVehiclePreview();
}

void RGVehicleDialog::playTimerEvent()
{
  updateVehiclePreview();
  mTimerCounter++;
}

void RGVehicleDialog::updateVehiclePreview()
{
  QPixmap pm(200,200);
  pm.fill(Qt::transparent);
  QPainter painter(&pm);
  painter.setPen(mPen);
  QPixmap pmVehicle=mVehicleList->getVehicle(mCurrentVehicleId)->getPixmap(mTimerCounter*mPlayTimer->interval());
  //Draw vehicle in the center
  int px = pm.size().width()/2 - pmVehicle.size().width() / 2;
  int py = pm.size().height()/2 - pmVehicle.size().height() / 2;
  painter.drawLine(pm.size().width()/2,pm.size().height()/2,0,pm.size().height()/2);
  painter.drawPixmap(px,py,pmVehicle);
  ui.vehiclePreviewLabel->setPixmap(pm);
  ui.vehicleListWidget->currentItem()->setIcon(QIcon(mVehicleList->getVehicle(mCurrentVehicleId)->getPixmapAtSize(40)));
}
