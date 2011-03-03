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

#include "RGMainWindow.h"
//#include "RGMapWidget.h"
#include "RGSettings.h"
#include "RGSettingsDialog.h"
#include "RGGoogleMap.h"
#include "RGVehicleDialog.h"
#include "RGVehicleList.h"
#include "RGEncVideo.h"
#include "RGEncFFmpeg.h"
#include "RGEncBmp2avi.h"
#include "RGRoute2.h"
#include "RGViewWidget.h"

#include "ui_routegen.h"

//Defined in main.cpp
extern const QString applicationName;

RGMainWindow::RGMainWindow(QWidget *parent)
  :QMainWindow(parent)
{
  //Set currentPath
  QDir::setCurrent(QCoreApplication::applicationDirPath());
  #ifdef Q_WS_X11
  QDir vehicleDir = QDir::currentPath() + "/vehicles";
  if(!vehicleDir.exists()){
    QDir::setCurrent(QDir::cleanPath(QCoreApplication::applicationDirPath()+"/../share/routegen"));
  }
  #endif

  setWindowIcon (QIcon(":/icons/icons/mapgen.png"));
  Ui::MainWindow ui;
  ui.setupUi(this);

  //Store some ui components locally
  actionOpen_image = ui.actionOpen_image;
  action_Quit = ui.action_Quit;
  actionSave_image = ui.actionSave_image;
  actionImport_Google_Map = ui.actionImport_Google_Map;
  actionDraw_mode = ui.actionDraw_mode;
  actionNew_route = ui.actionNew_route;
  action_Undo = ui.action_Undo;
  //action_Redo = ui.action_Redo;
  actionGenerate_map = ui.actionGenerate_map;
  actionPlayback = ui.actionPlayback;
  actionStop = ui.actionStop;
  /*mPenSizeSB = ui.penSizeSB;
  mRouteColorPB = ui.routeColorPB;
  mLineStyleCB = ui.lineStyleCB;
  mVehicleCB = ui.vehicleCB;
  mVehicleSettingsPB = ui.vehicleSettingsPB;
  mInterpolationCB = ui.interpolationCB;
  mSmoothPathCB = ui.smoothPathCB;
  mRouteTimeSB = ui.routeTimeSB;*/

  action_Undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  //action_Redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

  //Qt designer by default creates a widget in the QScrollArea, but we want our own widget inside it
  //delete ui.scrollAreaWidgetContents;
  //mRGMapWidget = new RGMapWidget();
  //ui.scrollArea->setWidget(mRGMapWidget);
  mView = new RGViewWidget();
  ui.centralwidget->layout()->addWidget(mView);


  actionStop->setEnabled(false);
  actionGenerate_map->setEnabled(false);
  actionPlayback->setEnabled(false);
  actionSave_image->setEnabled(false);
  actionDraw_mode->setEnabled(false);
  actionNew_route->setEnabled(false);
  action_Undo->setEnabled(false);
  /*

  //action_Redo->setEnabled(false);
  mVehicleSettingsPB->setEnabled(true);

  QObject::connect(mRGMapWidget, SIGNAL(busy(bool)),
                   this, SLOT(blockUserInteraction(bool)));

  QObject::connect(mRGMapWidget, SIGNAL(canGenerate(bool)),
                   this, SLOT(enableGenerateActions(bool)));

  QObject::connect(mRGMapWidget, SIGNAL(drawModeChanged(bool)),
                   this, SLOT(handleDrawModeChanged(bool)));

  Qt::PenStyle penStyle = (Qt::PenStyle) RGSettings::getPenStyle();

  //Create style icons for route style combobox
  mLineStyleCB->setIconSize(QSize(40, 10));
  mLineStyleCB->addItem(createIconForStyle(Qt::SolidLine),      QString(), QVariant((unsigned) Qt::SolidLine));
  mLineStyleCB->addItem(createIconForStyle(Qt::DashLine),       QString(), QVariant((unsigned) Qt::DashLine));
  mLineStyleCB->addItem(createIconForStyle(Qt::DotLine),        QString(), QVariant((unsigned) Qt::DotLine));
  mLineStyleCB->addItem(createIconForStyle(Qt::DashDotLine),    QString(), QVariant((unsigned) Qt::DashDotLine));
  mLineStyleCB->addItem(createIconForStyle(Qt::DashDotDotLine), QString(), QVariant((unsigned) Qt::DashDotDotLine));
  mLineStyleCB->addItem(createIconForStyle(Qt::NoPen),          QString(), QVariant((unsigned) Qt::NoPen));

  //Enum mapping starts counting at 1 (see Qt::PenStyle definition)
  mLineStyleCB->setCurrentIndex(penStyle - 1);

  mRouteColorPB->setAutoFillBackground(true);
  mRouteColorPB->setFlat(true);
  //Restore some initial settings
  QColor penCol = RGSettings::getPenColor();
  QPalette pal = mRouteColorPB->palette();
  pal.setColor(QPalette::Button, penCol);
  mRouteColorPB->setPalette(pal);

  int penSize = RGSettings::getPenSize();
  mPenSizeSB->setValue(penSize);
  setPen();

  mInterpolationCB->setChecked(RGSettings::getTotalTimeMode());
  mSmoothPathCB->setChecked(RGSettings::getSmoothPathMode());
  mRouteTimeSB->setValue(RGSettings::getRoutePlayTime());

  mVehicleList = new RGVehicleList();
  for (int i=0;i<mVehicleList->count();++i){
    mVehicleCB->addItem(QIcon(mVehicleList->getVehicle(i)->getPixmapAtSize(16)),mVehicleList->getVehicle(i)->getName());
  }
  mVehicleCB->setCurrentIndex(mVehicleList->getCurrentVehicleId());
  mRGMapWidget->setVehicle(*mVehicleList->getVehicle(mVehicleList->getCurrentVehicleId()));
  */
  //Video Encoder:
  #ifdef Q_WS_WIN
    mVideoEncoder = new RGEncBmp2avi();
  #else
    mVideoEncoder = new RGEncFFmpeg();
  #endif
  if(mVideoEncoder->exists())
    qDebug()<<"encoder found !";

  mRoute= new RGRoute2();
  ui.routeProperties->insertWidget(0,mRoute->widgetSettings());
  mRoute->setZValue(1);
  mRoute->setSmoothCoef(RGSettings::getSmoothLength());
  mView->addRoute(mRoute);
  QObject::connect(mRoute, SIGNAL(canGenerate(bool)),
                   this, SLOT(enableGenerateActions(bool)));
  QObject::connect(mView, SIGNAL(playbackStopped(bool)),
                   this, SLOT(enableGenerateActions(bool)));
}

void RGMainWindow::on_actionOpen_image_triggered(bool checked)
{
  Q_UNUSED(checked);
  QString lastOpenDir = RGSettings::getLastOpenDir();
  
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  lastOpenDir,
                                                  tr("Images (*.bmp *.jpg *.gif *.png *.tif)"));
  if (!fileName.isNull()){
    QPixmap pm(fileName);
    if (pm.isNull()) QMessageBox::critical (this, "Oops", "Could not load image");
    else{
      //TODO:Duplicated in on_actionImport_Google_Map_triggered
      //mRGMapWidget->loadImage(pm);
      mView->loadImage(pm);
      actionSave_image->setEnabled(true);
      actionDraw_mode->setEnabled(true);
      actionNew_route->setEnabled(true);
      RGSettings::setLastOpenDir(fileName);
    }
  }
}

void RGMainWindow::on_actionSave_image_triggered(bool checked)
{
  Q_UNUSED(checked);

  QString lastSaveDir = RGSettings::getLastSaveDir();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                  lastSaveDir,
                                                  tr("Images (*.bmp *.jpg)"));

  if (!fileName.isNull()){
    QPixmap pm = QPixmap();//mRGMapWidget->getImage();
    bool result = pm.save (fileName);
    if (!result) QMessageBox::critical (this, "Oops", "Problems saving file");
    RGSettings::setLastSaveDir(fileName);
  }
}

void RGMainWindow::on_action_Undo_triggered(bool)
{
  //mRGMapWidget->undo();
}

void RGMainWindow::on_action_Redo_triggered(bool)
{
  //mRGMapWidget->redo();
}

void RGMainWindow::on_actionPreferences_triggered(bool)
{
  RGSettingsDialog rgsettings(mVideoEncoder);

  if(rgsettings.exec()==QDialog::Accepted){
    mVideoEncoder->saveInSettings();
    mRoute->setSmoothCoef(rgsettings.getSmoothCoef());
    mRoute->setIconlessBeginEndFrames(rgsettings.getIconlessBeginEndFrames());
    //mRGMapWidget->updateRouteParametersFromSettings();
  }
  else
    mVideoEncoder->updateFromSettings();
}

void RGMainWindow::on_actionImport_Google_Map_triggered(bool)
{
  RGGoogleMap gm(this);
  if (gm.exec() == QDialog::Accepted){
    QPixmap map = gm.getMap();
    if (map.isNull())
      return;

    //Yes, lastOpenDir, because lastSaveDir is used to save map files
    //from the main window
    QString lastSaveDir = RGSettings::getLastOpenDir();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    lastSaveDir,
                                                    tr("Images (*.bmp)"));
    if (fileName.isEmpty())
      return;

	  map.save(fileName);

    //TODO:Duplicated in on_actionOpen_image_triggered
    //mRGMapWidget->loadImage(map);
    mView->loadImage(map);
    actionSave_image->setEnabled(true);
    actionDraw_mode->setEnabled(true);
    RGSettings::setLastOpenDir(fileName);
  }
}

void RGMainWindow::on_actionDraw_mode_triggered(bool checked)
{
  //if (checked) mRGMapWidget->startDrawMode();
  //else         mRGMapWidget->endDrawMode();
  mRoute->setEditMode(checked);
}

void RGMainWindow::on_actionNew_route_triggered(bool)
{
  //mRGMapWidget->startNewRoute();
  if(!actionDraw_mode->isChecked())
    actionDraw_mode->trigger();
  mRoute->clearPath();
  enableGenerateActions(false);
}

void RGMainWindow::on_actionPlayback_triggered(bool checked)
{
  Q_UNUSED(checked);
  mRoute->setEditMode(false);
  actionDraw_mode->setChecked(false);
  mView->play();
  actionStop->setEnabled(true);
}

void RGMainWindow::on_actionStop_triggered(bool checked)
{
  Q_UNUSED(checked);
  mView->stop();
  actionStop->setEnabled(false);
}

void RGMainWindow::on_actionGenerate_map_triggered(bool checked)
{
  Q_UNUSED(checked);
  bool generateBMPOK = false;
  QString lastGenDir = RGSettings::getLastGenDir();

  //Calculate MB the full movie (all uncompressed BMP's + AVI) will take
  //(This is a rough proximation, but the calculated number is always higher,
  // so we're safe)
  int sizeEstimate = 4555;//int ((mRGMapWidget->getNoFrames() * mRGMapWidget->getMapSize()) / 1048576.0) * 2;
  QString dirInfoText = QString("Select an empty directory on a drive with at least ") + 
                        QString::number(sizeEstimate) +
                        QString(" MB of free diskspace, where the map should be generated.");
  QString dir = QFileDialog::getExistingDirectory(this,
                                                  dirInfoText,
                                                  lastGenDir,
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);

  if (!dir.isNull()){
    //First generate the bmp files into a directory
    QDir qDir(dir);
    //Directory should be empty (i.e. contain 2 items: current dir and upper dir)
    if (qDir.count() == 2 ||
        QMessageBox::question (this, "Directory not empty", "Directory not empty, continue anyway?",
                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes){
      RGSettings::setLastGenDir(dir);
      generateBMPOK = mView->generateMovie(dir, QString("map"), mGeneratedBMPs);
    }

    if (generateBMPOK) {
      blockUserInteraction(true);
      QObject::connect(mVideoEncoder,SIGNAL(movieGenerationFinished()),this,SLOT(movieGenerationFinished()));
      mVideoEncoder->generateMovie(dir, QString("map"));
    }
  }
}

void RGMainWindow::on_action_Tutorial_triggered(bool checked)
{
  Q_UNUSED(checked);
  QTextBrowser *te = new QTextBrowser ();
  te->setWindowTitle("Route Generator Tutorial");
  te->setWindowIcon (QIcon(":/icons/icons/mapgen.png")); 
  te->setAttribute(Qt::WA_DeleteOnClose);
  te->setSearchPaths(QStringList(QDir::currentPath()));
  te->setSource(QUrl("doc/tutorial.html"));
  te->resize(800,700);
  te->show();
}

void RGMainWindow::on_action_About_triggered(bool checked)
{
  Q_UNUSED(checked);
  QString txt = QString(
      "<html>"
      "<center>"
      "<p><b>") + applicationName + QString(" Copyright (C) 2008-2011  Michiel Jansen </b></p>"
                                            "<p>This program comes with ABSOLUTELY NO WARRANTY</p>"
                                            "This is free software, and you are welcome to redistribute it "
                                            "under certain conditions; see LICENSE file for details.</p>"
                                            "<p>This program was developed using the GPL version of Qt 4.7.0<br>"
                                            "(Copyright (C) 2008-2011 Nokia Corporation.),<br>"
                                            "Qt can be <a href=\"http://qt.nokia.com/downloads\"> downloaded </a>"
                                            "from the <a href=\"http://qt.nokia.com\">Nokia Qt</a> website. </p>"
                                            "<p>The conversion from BMP to AVI is provided by:<br>"
                                            "<i>bmp2avi Copyright (C)) Paul Roberts 1996 - 1998</i></p>"
                                            "<p>For more information or questions about Route Generator you can "
                                            "<a href=\"mailto:info@routegenerator.net\">contact</a> me by e-mail.</p>"
                                            "</center>"
                                            "</html>");
  QMessageBox::about (this, "About Route Generator", txt );
}


void RGMainWindow::on_action_Quit_triggered(bool checked)
{
  Q_UNUSED(checked);
  qApp->quit();
}

void RGMainWindow::blockUserInteraction(bool busy)
{
  //mRGMapWidget->setBusy(busy);

  actionOpen_image->setEnabled(!busy);
  action_Quit->setEnabled(!busy);
  actionSave_image->setEnabled(!busy);
  actionDraw_mode->setEnabled(!busy);
  actionNew_route->setEnabled(!busy);
  actionGenerate_map->setEnabled(!busy);
  actionPlayback->setEnabled(!busy);
  actionImport_Google_Map->setEnabled(!busy);
  //Stop is only enabled while playing back
  actionStop->setEnabled(false);
}

void RGMainWindow::enableGenerateActions(bool val)
{
  actionGenerate_map->setEnabled(val);
  actionPlayback->setEnabled(val);
  action_Undo->setEnabled(val);
  actionStop->setEnabled(false);
  //TODO: Redo not implemented yet, so keep disabled for now
}

void RGMainWindow::movieGenerationFinished()
{
  qDebug()<<"Movie generation finished";
  if (RGSettings::getDeleteBMPs()) {
    //Delete generated BMP's except for first and last frame (user might want to use them)
    for (int i = 1; i < mGeneratedBMPs.size() - 1; i++) {
      if (!QFile::remove(mGeneratedBMPs[i])) {
        QMessageBox::critical (this, "Error", "Unable to delete generated BMP's! No permissions?");
        break;
      }
    }
  }
  blockUserInteraction(false);
  mVideoEncoder->disconnect();
}

/*QIcon RGMainWindow::createIconForStyle(Qt::PenStyle style)
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

void RGMainWindow::setPen()
{
  QPalette pal = mRouteColorPB->palette();
  QColor color = pal.color(QPalette::Button);
  int size = mPenSizeSB->value();
  QVariant data = mLineStyleCB->itemData(mLineStyleCB->currentIndex());
  Qt::PenStyle style = (Qt::PenStyle) data.toInt();
  mRGMapWidget->setPen(color,size,style);
}*/
