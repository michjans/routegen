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

#include "RGVehicleDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QStandardPaths>

RGVehicleDialog::RGVehicleDialog(QWidget* parent, RGVehicleList* vehicleList, const QPen& pen)
    : QDialog(parent),
      mVehicleList(vehicleList),
      mVehicleOrigin(nullptr),
      mPlayTimer(nullptr),
      mTimerCounter(0)
{
    ui.setupUi(this);

    //Manual connection, because somehow the generated connect code from the ui file dit not produce the "qOverload...", function, which is required here
    connect(ui.angleSlider, &QSlider::valueChanged, ui.angleValueLabel, qOverload<int>(&QLabel::setNum));

    for (int i = 0; i < vehicleList->count(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem(QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(40)),
                                                    mVehicleList->getVehicle(i)->getName()); //QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(40)),
        ui.vehicleListWidget->addItem(item);
    }

    if (mPlayTimer == nullptr)
    {
        mPlayTimer = new QTimer(this);
        QObject::connect(mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerEvent()));
    }

    ui.vehiclePreview->setRenderHint(QPainter::Antialiasing);
    ui.vehiclePreview->setRenderHint(QPainter::SmoothPixmapTransform);
    ui.vehiclePreview->setMinimumHeight(400 + 2 * ui.vehiclePreview->frameWidth());
    ui.vehiclePreview->setMinimumWidth(400 + 2 * ui.vehiclePreview->frameWidth());
    mScene = new QGraphicsScene(0, 0, 400, 400);
    ui.vehiclePreview->setScene(mScene);

    mScene->addLine(200, 200, 0, 200, pen); //QGraphicsLineItem *line=

    ui.vehicleListWidget->setCurrentRow(mVehicleList->getCurrentVehicleId());
}

RGVehicleDialog::~RGVehicleDialog()
{
    //qDebug() << "RGVehicleDialog::~RGVehicleDialog";
}

void RGVehicleDialog::accept()
{
    mScene->removeItem(mVehicleList->getCurrentVehicle());
    if (mVehicleOrigin)
        delete mVehicleOrigin;
    QDialog::accept();
}

void RGVehicleDialog::reject()
{
    mScene->removeItem(mVehicleList->getCurrentVehicle());
    if (mVehicleOrigin)
        delete mVehicleOrigin;
    QDialog::reject();
}

void RGVehicleDialog::on_addVehiclePB_clicked(bool)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image/icon file to add as custom vehicle"),
                                                    QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Images (*.png *.jpg *.gif *.svg)"));
    if (!fileName.isNull())
    {
        QString errStr;
        RGVehicle* vehicle = mVehicleList->addCustomVehicle(fileName, errStr);
        if (vehicle != nullptr)
        {
            QListWidgetItem* item = new QListWidgetItem(QIcon(vehicle->getPixmapAtSize(40)), vehicle->getName());
            ui.vehicleListWidget->addItem(item);
        }
        else
        {
            QMessageBox::warning(this, "Unable to add custom vehicle", errStr);
        }
    }
}

void RGVehicleDialog::on_removeVehiclePB_clicked(bool)
{
    QMessageBox::StandardButton answer = QMessageBox::question(this, "Remove custom vehicle", "Are you sure you want to remove this custom vehicle?",
                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::No)
        return;

    mVehicleList->removeCustomVehicle(mVehicleList->getCurrentVehicle());

    //Remove currently selected vehicle from list as well
    //Current row will be changed automatically by delete, and currentRowChanged will occur, but
    //VehicleOrigin will implicitely be deleted with the item as well, so reset pointer.
    mVehicleOrigin = nullptr;
    delete ui.vehicleListWidget->currentItem();
}

void RGVehicleDialog::on_vehicleListWidget_currentRowChanged(int currentRow)
{
    if (mPlayTimer->isActive())
        mPlayTimer->stop();
    mTimerCounter = 0;
    if (mVehicleOrigin)
        delete mVehicleOrigin;
    if (mScene == mVehicleList->getCurrentVehicle()->scene())
        mScene->removeItem(mVehicleList->getCurrentVehicle());
    mVehicleList->getCurrentVehicle()->setVisible(false);

    //set new Vehicle
    mVehicleList->setCurrentVehicleId(currentRow);
    RGVehicle* vehicle = mVehicleList->getCurrentVehicle();
    ui.sizeSB->setValue(vehicle->getSize());
    ui.angleSlider->setValue(vehicle->getStartAngle());
    ui.mirrorCB->setChecked(vehicle->getMirror());
    ui.rotateCB->setChecked(vehicle->acceptsRotation());

    mScene->addItem(vehicle);

    //add Vehicle Origin Point
    mVehicleOrigin = new RGVehicleOriginPt(vehicle);

    //center vehicle
    vehicle->setRotation(0);
    vehicle->setPos(200, 200);
    vehicle->setVisible(true);

    ui.removeVehiclePB->setEnabled(vehicle->isCustom());

    if (mVehicleList->getCurrentVehicle()->getDelay() > 0)
    {
        mPlayTimer->setInterval(mVehicleList->getCurrentVehicle()->getDelay());
        mPlayTimer->start();
    }
}

void RGVehicleDialog::on_sizeSB_valueChanged(int size)
{
    mVehicleList->getCurrentVehicle()->setSize(size);
}

void RGVehicleDialog::on_angleSlider_valueChanged(int angle)
{
    mVehicleList->getCurrentVehicle()->setStartAngle(angle);
    ui.vehicleListWidget->currentItem()->setIcon(QIcon(mVehicleList->getCurrentVehicle()->getPixmapAtSize(40)));
}

void RGVehicleDialog::on_resetSizePB_clicked(bool)
{
    mVehicleList->getCurrentVehicle()->setSize(0);
    ui.sizeSB->setValue(mVehicleList->getCurrentVehicle()->getSize());
}

void RGVehicleDialog::on_mirrorCB_toggled(bool state)
{
    mVehicleList->getCurrentVehicle()->setMirror(state);
    ui.vehicleListWidget->currentItem()->setIcon(QIcon(mVehicleList->getCurrentVehicle()->getPixmapAtSize(40)));
}

void RGVehicleDialog::on_rotateCB_toggled(bool state)
{
    mVehicleList->getCurrentVehicle()->acceptRotation(state);
    ui.vehicleListWidget->currentItem()->setIcon(QIcon(mVehicleList->getCurrentVehicle()->getPixmapAtSize(40)));
}

void RGVehicleDialog::playTimerEvent()
{
    mTimerCounter++;
    mVehicleList->getCurrentVehicle()->setTime(mTimerCounter * mPlayTimer->interval());
}
