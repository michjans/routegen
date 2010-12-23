/*
    Copyright (C) 2008  Michiel Jansen

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

#include <QDir>
#include <QDebug>
#include <QIcon>
#include <QPainter>

#include "RGVehicleDialog.h"
#include "RGSettings.h"

RGVehicleDialog::RGVehicleDialog(QWidget *parent)
    : QDialog(parent)
{

    ui.setupUi(this);

    QDir vehicleDir = QDir::currentPath() + "/vehicles";
    QStringList filters;
    filters << "*.bmp" << "*.gif" << "*.png" << "*.jpg" << "*.tif";
    vehicleDir.setNameFilters(filters);
    QFileInfoList vehicles = vehicleDir.entryInfoList();
    RGVehicle *vehicle;
    vehicle= new RGVehicle();
    mList.append(vehicle);
    QListWidgetItem *item = new QListWidgetItem(QIcon(vehicle->getPixmap()),QString("None"));
    ui.vehicleListWidget->addItem(item);
    for (QFileInfoList::iterator it = vehicles.begin(); it != vehicles.end(); it++)
    {
        vehicle= new RGVehicle(it->absoluteFilePath(),RGSettings::getVehicleSize(it->baseName()),
                               RGSettings::getVehicleMirrored(it->baseName()),RGSettings::getVehicleAngle(it->baseName()));
        qDebug()<<"qSize du vehicle"<<vehicle->getSize();
        if (vehicle->getRawSize()==0)
        {
            delete vehicle;
            continue;
        }
        vehicle->setSize(40);
        QListWidgetItem *item = new QListWidgetItem(QIcon(vehicle->getPixmap()),it->baseName());
        vehicle->setSize(RGSettings::getVehicleSize(it->baseName()));
        ui.vehicleListWidget->addItem(item);
        mList.append(vehicle);
    }
    QString lastVehicleName=RGSettings::getLastVehicleName();
    if (lastVehicleName!=QString("None")){
        for (int i=0;i<ui.vehicleListWidget->count();i++)
        {
            if (ui.vehicleListWidget->item(i)->text()==lastVehicleName){
                ui.vehicleListWidget->setCurrentItem(ui.vehicleListWidget->item(i));
                break;
            }
        }
    }
    //TODO : error if no vehicles
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
    for (int i=0;i<mList.count();++i)
    {
        delete mList.at(i);
    }
}

RGVehicle RGVehicleDialog::getVehicle()
{
    if(mList.count()>mVehicleId)
        return *(mList.at(mVehicleId));
    return RGVehicle();
}

QList<QIcon> RGVehicleDialog::getIconList()
{
  QList<QIcon> listicon;
  for (int i=0;i<mList.count();++i)
  {
      listicon.append(ui.vehicleListWidget->item(i)->icon());
  }
  return listicon;
}

QStringList RGVehicleDialog::getNameList()
{
  QStringList listname=QStringList();
  for (int i=0;i<mList.count();++i)
  {
      listname.append(ui.vehicleListWidget->item(i)->text());
  }
  return listname;
}

int RGVehicleDialog::count()
{
    return mList.count();
}

void RGVehicleDialog::accept()
{
    mVehicleId = ui.vehicleListWidget->currentRow();
    RGSettings::setVehicleAngle(ui.vehicleListWidget->currentItem()->text(),mList.at(mVehicleId)->getStartAngle());
    RGSettings::setVehicleSize(ui.vehicleListWidget->currentItem()->text(),mList.at(mVehicleId)->getSize());
    RGSettings::setVehicleMirrored(ui.vehicleListWidget->currentItem()->text(),mList.at(mVehicleId)->getMirror());
    RGSettings::setLastVehicleName(ui.vehicleListWidget->currentItem()->text());
    QDialog::accept();
}

void RGVehicleDialog::on_vehicleListWidget_currentRowChanged(int currentRow)
{
    mVehicleId = currentRow;
    ui.sizeSB->setValue(mList.at(mVehicleId)->getSize());
    ui.angleSB->setValue(mList.at(mVehicleId)->getStartAngle());
    ui.mirrorCB->setChecked(mList.at(mVehicleId)->getMirror());
    updateVehiclePreview();
}

void RGVehicleDialog::on_sizeSB_valueChanged(int size)
{
    mList.at(mVehicleId)->setSize(size);
    updateVehiclePreview();
}

void RGVehicleDialog::on_angleSB_valueChanged(int angle)
{
    mList.at(mVehicleId)->setStartAngle(angle);
    updateVehiclePreview();
}

void RGVehicleDialog::on_resetSizePB_clicked(bool)
{
    mList.at(mVehicleId)->setSize(mList.at(mVehicleId)->getRawSize());
    ui.sizeSB->setValue(mList.at(mVehicleId)->getSize());
    updateVehiclePreview();
}

void RGVehicleDialog::on_resetAnglePB_clicked(bool)
{
    mList.at(mVehicleId)->setStartAngle(0);
    ui.angleSB->setValue(mList.at(mVehicleId)->getStartAngle());
    updateVehiclePreview();
}

void RGVehicleDialog::on_mirrorCB_toggled(bool state)
{
    mList.at(mVehicleId)->setMirror(state);
    updateVehiclePreview();
}

void RGVehicleDialog::updateVehiclePreview()
{
    //QPixmap pmvehicle=mList.at(mVehicleId)->getPixmap();

    ui.vehiclePreviewLabel->setPixmap(mList.at(mVehicleId)->getPixmap());
}
