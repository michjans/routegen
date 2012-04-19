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

#include "RGRouteUi.h"
#include "ui_routeui.h"

#include "RGVehicleDialog.h"
#include "RGSettings.h"
#include "RGVehicleList.h"

#include <QDebug>
#include <QPainter>
#include <QColorDialog>

RGRouteUi::RGRouteUi(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::routeUi)
{
  ui->setupUi(this);

  //fill Ui

  //Create style icons for route style combobox
  ui->lineStyleCB->setIconSize(QSize(40, 10));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::SolidLine),      QString(), QVariant((unsigned) Qt::SolidLine));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::DashLine),       QString(), QVariant((unsigned) Qt::DashLine));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::DotLine),        QString(), QVariant((unsigned) Qt::DotLine));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::DashDotLine),    QString(), QVariant((unsigned) Qt::DashDotLine));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::DashDotDotLine), QString(), QVariant((unsigned) Qt::DashDotDotLine));
  ui->lineStyleCB->addItem(createIconForStyle(Qt::NoPen),          QString(), QVariant((unsigned) Qt::NoPen));
  Qt::PenStyle penStyle = (Qt::PenStyle) RGSettings::getPenStyle();
  //Enum mapping starts counting at 1 (see Qt::PenStyle definition)
  ui->lineStyleCB->setCurrentIndex(penStyle - 1);

  //Set colorBox:
  ui->routeColorPB->setAutoFillBackground(true);
  ui->routeColorPB->setFlat(true);
  //Restore some initial settings
  QColor penCol = RGSettings::getPenColor();
  QPalette pal = ui->routeColorPB->palette();
  pal.setColor(QPalette::Button, penCol);
  ui->routeColorPB->setPalette(pal);

  int penSize = RGSettings::getPenSize();
  ui->penSizeSB->setValue(penSize);

  ui->totalTimeCB->setChecked(RGSettings::getTotalTimeMode());
  ui->smoothPathCB->setChecked(RGSettings::getSmoothPathMode());
  ui->routeTimeSB->setValue(RGSettings::getRoutePlayTime());

  //Connects signals :
  //QObject::connect(ui->vehicleCB,SIGNAL(activated(int)),this,SIGNAL(vehicleChanged(int)));

}

RGRouteUi::~RGRouteUi()
{
  delete ui;
}

void RGRouteUi::setVehicleList(RGVehicleList *vehicleList)
{
  //set vehicleList and fill comboBox
  mVehicleList = vehicleList;
  reFillVehicleComboFromVehicleList();
  ui->vehicleCB->setCurrentIndex(mVehicleList->getCurrentVehicleId());
}

void RGRouteUi::init()
{
  //send initial signals
  setPen();
  emit totalTimeChecked(ui->totalTimeCB->isChecked());
  emit smoothPathChecked(ui->smoothPathCB->isChecked());
  emit routeTimeChanged(ui->routeTimeSB->value());
}

void RGRouteUi::on_vehicleSettingsPB_clicked(bool)
{
  //get Pen properties:
  QPen pen((Qt::PenStyle) ui->lineStyleCB->itemData(ui->lineStyleCB->currentIndex()).toInt());
  pen.setColor(ui->routeColorPB->palette().color(QPalette::Button));
  pen.setWidth(ui->penSizeSB->value());

  int lastVehicleId=mVehicleList->getCurrentVehicleId();

  RGVehicleDialog vehicleDialog(this,mVehicleList,pen);
  bool vehicleSettingsChanged = (vehicleDialog.exec() == QDialog::Accepted);

  //A littlebit of a design flaw, but the user could have added/removed a custom vehicle
  //and clicked cancel afterwards. However, the custom vehicle will already have been added/deleted,
  //so we always have to check this, indepenent of the user clicked ok/cancel
  if (mVehicleList->count() != ui->vehicleCB->count() || vehicleSettingsChanged)
  {
    //Always refill the combo, either vehicles have been added/removed or
    //orientations, etc. have been changed. In both cases we want to do an update.
    reFillVehicleComboFromVehicleList();
    ui->vehicleCB->setCurrentIndex(mVehicleList->getCurrentVehicleId());
    if (vehicleSettingsChanged)
    {
      //Only do this when user clicked ok
      mVehicleList->saveVehiclesSettings();
    }
  }
  else{
    mVehicleList->loadVehiclesSettings();
    mVehicleList->setCurrentVehicleId(lastVehicleId);
  }
  emit vehicleChanged();
}

void RGRouteUi::on_penSizeSB_valueChanged(int size)
{
  //Store
  RGSettings::setPenSize(size);
  setPen();
}

void RGRouteUi::on_lineStyleCB_activated(int idx)
{
  QVariant data = ui->lineStyleCB->itemData(idx);
  Qt::PenStyle style = (Qt::PenStyle) data.toInt();
  RGSettings::setPenStyle(style);
  setPen();
}

void RGRouteUi::on_totalTimeCB_toggled(bool checked)
{
  RGSettings::setTotalTimeMode(checked);
  emit totalTimeChecked(checked);
}

void RGRouteUi::on_smoothPathCB_toggled(bool checked)
{
  RGSettings::setSmoothPathMode(checked);
  emit smoothPathChecked(checked);
}

void RGRouteUi::on_routeTimeSB_valueChanged(int time)
{
  RGSettings::setRoutePlayTime(time);
  emit routeTimeChanged(time);
}

void RGRouteUi::on_vehicleCB_currentIndexChanged(int index)
{
  mVehicleList->setCurrentVehicleId(index);
  emit vehicleChanged();
}

void RGRouteUi::on_routeColorPB_clicked(bool)
{
  QPalette pal = ui->routeColorPB->palette();
  QColor newCol = QColorDialog::getColor ( pal.color(QPalette::Button), this );
  if (newCol.isValid()) {
    QPalette pal = ui->routeColorPB->palette();
    pal.setColor(QPalette::Button, newCol);
    ui->routeColorPB->setPalette(pal);
    //Store
    RGSettings::setPenColor(newCol);

    //update mLineStyleCB
    int penStyle = ui->lineStyleCB->currentIndex();
    ui->lineStyleCB->clear();
    ui->lineStyleCB->addItem(createIconForStyle(Qt::SolidLine),      QString(), QVariant((unsigned) Qt::SolidLine));
    ui->lineStyleCB->addItem(createIconForStyle(Qt::DashLine),       QString(), QVariant((unsigned) Qt::DashLine));
    ui->lineStyleCB->addItem(createIconForStyle(Qt::DotLine),        QString(), QVariant((unsigned) Qt::DotLine));
    ui->lineStyleCB->addItem(createIconForStyle(Qt::DashDotLine),    QString(), QVariant((unsigned) Qt::DashDotLine));
    ui->lineStyleCB->addItem(createIconForStyle(Qt::DashDotDotLine), QString(), QVariant((unsigned) Qt::DashDotDotLine));
    ui->lineStyleCB->addItem(createIconForStyle(Qt::NoPen),          QString(), QVariant((unsigned) Qt::NoPen));
    ui->lineStyleCB->setCurrentIndex(penStyle);
  }
  setPen();
}

void RGRouteUi::blockEssentialControls(bool block)
{
  ui->vehicleSettingsPB->setDisabled(block);
  ui->routeTimeSB->setDisabled(block);
  ui->totalTimeCB->setDisabled(block);
}

QIcon RGRouteUi::createIconForStyle(Qt::PenStyle style)
{
  QPixmap pm(40, 10);
  pm.fill(Qt::transparent);
  QPainter painter(&pm);
  QBrush brush(RGSettings::getPenColor(), Qt::SolidPattern);
  QPen pen(brush, 2, style, Qt::FlatCap, Qt::RoundJoin);
  painter.setPen (pen);
  painter.drawLine(0, 5, 39, 5);
  return QIcon(pm);
}

void RGRouteUi::setPen()
{
  QPen pen;
  pen.setWidth(ui->penSizeSB->value());
  pen.setColor(ui->routeColorPB->palette().color(QPalette::Button));
  pen.setStyle((Qt::PenStyle) ui->lineStyleCB->itemData(ui->lineStyleCB->currentIndex()).toInt());
  pen.setCapStyle(Qt::FlatCap);
  pen.setJoinStyle(Qt::RoundJoin);
  emit penChanged(pen);
}


void RGRouteUi::reFillVehicleComboFromVehicleList()
{
  ui->vehicleCB->blockSignals(true);//block to stop calling CurrentIndexChanged while vehicle are added
  ui->vehicleCB->clear();
  for (int i=0;i<mVehicleList->count();++i){
    ui->vehicleCB->addItem(QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(16)),mVehicleList->getVehicle(i)->getName());//QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(16)),
  }
  ui->vehicleCB->blockSignals(false);
}
