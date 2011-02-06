#include "RGRouteUi.h"
#include "ui_routeui.h"

#include "RGVehicleDialog.h"
#include "RGSettings.h"
#include <QPainter>

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
  setPen();

  ui->totalTimeCB->setChecked(RGSettings::getInterpolationMode());
  ui->smoothPathCB->setChecked(RGSettings::getSmoothPathMode());
  ui->routeTimeSB->setValue(RGSettings::getRoutePlayTime());
}

RGRouteUi::~RGRouteUi()
{
  delete ui;
}

void RGRouteUi::setVehicleList(RGVehicleList *vehicleList)
{
  //set vehicleList and fill comboBox
  mVehicleList = vehicleList;
  for (int i=0;i<mVehicleList->count();++i){
    ui->vehicleCB->addItem(QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(16)),mVehicleList->getVehicle(i)->getName());
  }
  ui->vehicleCB->setCurrentIndex(mVehicleList->getCurrentVehicleId());
}

void RGRouteUi::on_vehicleSettingsPB_clicked(bool)
{
  //get Pen properties:
  QPen pen((Qt::PenStyle) ui->lineStyleCB->itemData(ui->lineStyleCB->currentIndex()).toInt());
  pen.setColor(ui->routeColorPB->palette().color(QPalette::Button));
  pen.setWidth(ui->penSizeSB->value());

  RGVehicleDialog vehicleDialog(this,mVehicleList);
  vehicleDialog.setPen(pen);
  if (vehicleDialog.exec() == QDialog::Accepted){
    ui->vehicleCB->setCurrentIndex(mVehicleList->getCurrentVehicleId());
    //mRGMapWidget->setVehicle(*mVehicleList->getVehicle(mVehicleList->getCurrentVehicleId()));
  }
  //update icons of the comboBox
  for(int i=0;i<mVehicleList->count();i++){
    ui->vehicleCB->setItemIcon(i,QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(16)));
  }
}

QIcon RGRouteUi::createIconForStyle(Qt::PenStyle style)
{
  QPixmap pm(40, 10);
  pm.fill();
  QPainter painter(&pm);
  QBrush brush(RGSettings::getPenColor(), Qt::SolidPattern);
  QPen pen(brush, 2, style, Qt::FlatCap, Qt::RoundJoin);
  painter.setPen (pen);
  painter.drawLine(0, 5, 39, 5);
  return QIcon(pm);
}

void RGRouteUi::setPen()
{
  QPalette pal = ui->routeColorPB->palette();
  QColor color = pal.color(QPalette::Button);
  int size = ui->penSizeSB->value();
  QVariant data = ui->lineStyleCB->itemData(ui->lineStyleCB->currentIndex());
  Qt::PenStyle style = (Qt::PenStyle) data.toInt();
  emit penChanged(color,size,style);
}
