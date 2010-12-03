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


#include <QtGui>

#include "RGMainWindow.h"
#include "RGMapWidget.h"
#include "RGSettings.h"
#include "RGSettingsDialog.h"
#include "RGGoogleMap.h"

#include "ui_routegen.h"

//Defined in main.cpp
extern const QString applicationName;

RGMainWindow::RGMainWindow(QWidget *parent)
  :QMainWindow(parent),
  mDrawMirroredIcon(false),
  mStartAngle(0),
  mIgnoreSignals(false)
{
  
  setWindowIcon (QIcon(":/icons/icons/mapgen.png")); 
  Ui::MainWindow ui;
  ui.setupUi(this);

  //Store some ui components locally
  actionOpen_image = ui.actionOpen_image;
  action_Quit = ui.action_Quit;
  actionSave_image = ui.actionSave_image;
  actionDraw_mode = ui.actionDraw_mode;
  action_Undo = ui.action_Undo;
  //action_Redo = ui.action_Redo;
  actionGenerate_map = ui.actionGenerate_map;
  actionPlayback = ui.actionPlayback;
  actionStop = ui.actionStop;
  mIconCB = ui.iconCB;
  mMirrorCB = ui.mirrorIconCB;
  mAngleSB = ui.angleSB;
  mRouteColorPB = ui.routeColorPB;
  mLineStyleCB = ui.lineStyleCB;
  mVehiclePreviewLabel = ui.vehiclePreviewLabel;
  mInterpolationCB = ui.interpolationCB;
  mRouteTimeSB = ui.routeTimeSB;

  action_Undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  //action_Redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

  //Qt designer by default creates a widget in the QScrollArea, but we want our own widget inside it
  delete ui.scrollAreaWidgetContents;
  mRGMapWidget = new RGMapWidget();
  ui.scrollArea->setWidget(mRGMapWidget);

  actionSave_image->setEnabled(false);
  actionDraw_mode->setEnabled(false);
  action_Undo->setEnabled(false);
  //action_Redo->setEnabled(false);
  actionGenerate_map->setEnabled(false);
  actionPlayback->setEnabled(false);
  actionStop->setEnabled(false);
  mMirrorCB->setEnabled(false);
  mAngleSB->setEnabled(false);



  QObject::connect(ui.genFramesCB, SIGNAL(toggled ( bool )),
          mRGMapWidget, SLOT(setGenerateBeginEndFrames(bool)));

  QObject::connect(mRGMapWidget, SIGNAL(busy(bool)),
          this, SLOT(blockUserInteraction(bool)));

  QObject::connect(mRGMapWidget, SIGNAL(canGenerate(bool)),
          this, SLOT(enableGenerateActions(bool)));

  QObject::connect(mRGMapWidget, SIGNAL(drawModeChanged(bool)),
          this, SLOT(handleDrawModeChanged(bool)));

  mIconCB->addItem("None", QString());

  //Create vehicle icons from icons in vehicle dir
  QDir vehicleDir = QDir::currentPath() + "/vehicles";
  QStringList filters;
    filters << "*.bmp" << "*.gif" << "*.png" << "*.jpg" << "*.tif";
    vehicleDir.setNameFilters(filters); 
  QFileInfoList vehicles = vehicleDir.entryInfoList();
  for (QFileInfoList::iterator it = vehicles.begin(); it != vehicles.end(); it++)
  {
    QIcon veh = QIcon(it->absoluteFilePath());
    if (veh.isNull()) continue;
    mIconCB->addItem(veh, it->baseName(), it->absoluteFilePath());
  }

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
  mRGMapWidget->setPenStyle(penStyle);

  mRouteColorPB->setAutoFillBackground(true);
  mRouteColorPB->setFlat(true);

  //Restore some initial settings
  QColor penCol = RGSettings::getPenColor();
  QPalette pal = mRouteColorPB->palette();
  pal.setColor(QPalette::Button, penCol);
  mRouteColorPB->setPalette(pal);
  mRGMapWidget->setPenColor(penCol);

  int penSize = RGSettings::getPenSize();
  ui.penSizeSB->setValue(penSize);
  mRGMapWidget->setPenSize(penSize);

  mInterpolationCB->setChecked(RGSettings::getInterpolationMode());
  mRouteTimeSB->setValue(RGSettings::getRoutePlayTime());

}

void RGMainWindow::on_actionOpen_image_triggered(bool checked)
{
  Q_UNUSED(checked);
  QString lastOpenDir = RGSettings::getLastOpenDir();
  
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 lastOpenDir,
                                                 tr("Images (*.bmp *.jpg *.gif *.png *.tif)")); 
  if (!fileName.isNull())
  {
    QPixmap pm(fileName);
    if (pm.isNull()) QMessageBox::critical (this, "Oops", "Could not load image");
    else
    {
      //TODO:Duplicated in on_actionImport_Google_Map_triggered
      mRGMapWidget->loadImage(pm);
      actionSave_image->setEnabled(true);
      actionDraw_mode->setEnabled(true);
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

  if (!fileName.isNull())
  {
    QPixmap pm = mRGMapWidget->getImage();
    bool result = pm.save (fileName);
    if (!result) QMessageBox::critical (this, "Oops", "Problems saving file");
    RGSettings::setLastSaveDir(fileName);
  }
}

void RGMainWindow::on_action_Undo_triggered(bool)
{
  mRGMapWidget->undo();
}

void RGMainWindow::on_action_Redo_triggered(bool)
{
  mRGMapWidget->redo();
}

void RGMainWindow::on_actionPreferences_triggered(bool)
{
  RGSettingsDialog rgsettings;
  rgsettings.exec();
}

void RGMainWindow::on_actionImport_Google_Map_triggered(bool)
{
  RGGoogleMap gm(this);
  if (gm.exec() == QDialog::Accepted)
  {
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
    mRGMapWidget->loadImage(map);
    actionSave_image->setEnabled(true);
    actionDraw_mode->setEnabled(true);
    RGSettings::setLastOpenDir(fileName);
  }
}

void RGMainWindow::on_actionDraw_mode_triggered(bool checked)
{
  if (checked) mRGMapWidget->startDrawMode();
  else         mRGMapWidget->endDrawMode();
}

void RGMainWindow::on_actionPlayback_triggered(bool checked)
{
  Q_UNUSED(checked);
  mRGMapWidget->play();
  actionStop->setEnabled(true);
}

void RGMainWindow::on_actionStop_triggered(bool checked)
{
  Q_UNUSED(checked);
  mRGMapWidget->stop();
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
  int sizeEstimate = int ((mRGMapWidget->getNoFrames() * mRGMapWidget->getMapSize()) / 1048576.0) * 2;
  QString dirInfoText = QString("Select an empty directory on a drive with at least ") + 
                      QString::number(sizeEstimate) +
              QString(" MB of free diskspace, where the map should be generated.");
  QString dir = QFileDialog::getExistingDirectory(this,
            dirInfoText,
            lastGenDir,
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks); 

  if (!dir.isNull())
  {
    //First generate the bmp files into a directory
    QDir qDir(dir);
    //Directory should be empty (i.e. contain 2 items: current dir and upper dir)
    if (qDir.count() == 2 ||
      QMessageBox::question (this, "Directory not empty", "Directory not empty, continue anyway?",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
      RGSettings::setLastGenDir(dir);
      generateBMPOK = mRGMapWidget->generateMovie(dir, QString("map"), mGeneratedBMPs);
    }

    if (generateBMPOK) {
      //Now use bmp2avi to convert the bmp files into an avi file
      //location of bmp2avi should already be stored in the settings
      QString videoEncoder = RGSettings::getVideoEncoder();
      qWarning()<< "videoenc: " << videoEncoder;
      if (videoEncoder.isEmpty()){
          QString txt = QString(
            "<html>"
            "<p>"
            "Your route has been generated in the selected directory. "
            "Each frame is generated as a *.bmp file in that directory. "
            "</p>"
            "<p><b>NOTE: Since no video encoder is available, no avi file is generated!</b></p>"
            "</html>"
          );

          QMessageBox::information (this, "Map Generation Finished", txt );
          return;
      }
      QString videoEncoderName = RGSettings::getVideoEncExec();
      QString fps      = QString::number(RGSettings::getFps());
      QString outname  = RGSettings::getAviOutName();
      QString compress = RGSettings::getAviCompression();
      QString key      = QString::number(RGSettings::getKeyFrameRate());
      QStringList arguments;
      if (videoEncoder==QString("bmp2avi")){
          arguments << "-f" << fps << "-k" << key << "-o" << outname << "-c" << compress;
      }
      if (videoEncoder==QString("ffmpeg")){
          outname.append(".mpg");
          arguments << "-y" << "-f" << "image2" << "-i" << "map\%05d.bmp" << "-g" << key <<"-r"<<fps<< outname;
      }
      mVideoEncProcess = new QProcess(this);
      QObject::connect(mVideoEncProcess, SIGNAL(finished (int , QProcess::ExitStatus)),
                       this, SLOT(handleVideoEncProcessFinished(int , QProcess::ExitStatus)));
      QObject::connect(mVideoEncProcess, SIGNAL(error (QProcess::ProcessError)),
                       this, SLOT(handleVideoEncProcessError(QProcess::ProcessError)));
      mVideoEncProcess->setWorkingDirectory(dir);
      blockUserInteraction(true);
      mVideoEncProcess->start(videoEncoderName, arguments);
      mProcessWaitMessage = new QMessageBox(this);
      mProcessWaitMessage->setWindowTitle("One moment please...");
      mProcessWaitMessage->setText("Executing videoEncoder to convert BMP files to video file, one moment please...");
      mProcessWaitMessage->setStandardButtons(QMessageBox::NoButton);
      mProcessWaitMessage->setCursor(Qt::WaitCursor);
      mProcessWaitMessage->show();
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
              "<p><b>") + applicationName + QString(" Copyright (C) 2008-2010  Michiel Jansen </b></p>"
              "<p>This program comes with ABSOLUTELY NO WARRANTY</p>"
              "This is free software, and you are welcome to redistribute it "
              "under certain conditions; see LICENSE file for details.</p>"
              "<p>This program was developed using the GPL version of Qt 4.7.0<br>"
              "(Copyright (C) 2008-2010 Nokia Corporation.),<br>"
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

void RGMainWindow::on_iconCB_activated(int index)
{
  mMirrorCB->setEnabled(index != 0);
  mAngleSB->setEnabled(index != 0);

  mIgnoreSignals = true;

  //Restore settings for current vehicle
  if (index > 0) {
    mDrawMirroredIcon = RGSettings::getVehicleMirrored(mIconCB->currentText());
    mStartAngle = RGSettings::getVehicleAngle(mIconCB->currentText());
    mMirrorCB->setChecked(mDrawMirroredIcon);
    mAngleSB->setValue(mStartAngle);
  } else {
    mDrawMirroredIcon = false;
    mStartAngle = 0;
    mMirrorCB->setChecked(false);
    mAngleSB->setValue(0);
  }

  updateVehicleIcon(index);

  mIgnoreSignals = false;
}

void RGMainWindow::on_routeColorPB_clicked(bool)
{
  QColor newCol = QColorDialog::getColor ( mRGMapWidget->getPenColor(), this );
  if (newCol.isValid()) {
    mRGMapWidget->setPenColor(newCol);
    QPalette pal = mRouteColorPB->palette();
    pal.setColor(QPalette::Button, mRGMapWidget->getPenColor());
    mRouteColorPB->setPalette(pal);

    //Store
    RGSettings::setPenColor(newCol);
  }
}

void RGMainWindow::on_penSizeSB_valueChanged(int size)
{
  mRGMapWidget->setPenSize(size);
  //Store
  RGSettings::setPenSize(size);
}

void RGMainWindow::on_lineStyleCB_activated(int idx)
{
  QVariant data = mLineStyleCB->itemData(idx);

  Qt::PenStyle style = (Qt::PenStyle) data.toInt();

  mRGMapWidget->setPenStyle(style);
  RGSettings::setPenStyle(style);
}

void RGMainWindow::on_angleSB_valueChanged(int angle)
{
  if (mIgnoreSignals) return;
  
  mStartAngle = angle;

  //Store settings for current vehicle
  if (mIconCB->currentIndex() > 0) {
    RGSettings::setVehicleAngle(mIconCB->currentText(), angle);
  }

  updateVehicleIcon(mIconCB->currentIndex());
}

void RGMainWindow::on_mirrorIconCB_toggled(bool checked)
{
  if (mIgnoreSignals) return;

  mDrawMirroredIcon = checked;

  //Store settings for current vehicle
  if (mIconCB->currentIndex() > 0) {
    RGSettings::setVehicleMirrored(mIconCB->currentText(), checked);
  }

  updateVehicleIcon(mIconCB->currentIndex());
}

void RGMainWindow::on_interpolationCB_toggled(bool checked)
{
  mRGMapWidget->setInterpolationMode(checked);
}

void RGMainWindow::on_routeTimeSB_valueChanged(int time)
{
  mRGMapWidget->setRoutePlayTime(time);
}

void RGMainWindow::blockUserInteraction(bool busy)
{
  mRGMapWidget->setBusy(busy);

  actionOpen_image->setEnabled(!busy);
  action_Quit->setEnabled(!busy);
  actionSave_image->setEnabled(!busy);
  actionDraw_mode->setEnabled(!busy);
  actionGenerate_map->setEnabled(!busy);
  actionPlayback->setEnabled(!busy);
  mRouteColorPB->setEnabled(!busy);
  mLineStyleCB->setEnabled(!busy);
  //Stop is only enabled while playing back
  actionStop->setEnabled(false);
}

void RGMainWindow::enableGenerateActions(bool val)
{
  actionGenerate_map->setEnabled(val);
  actionPlayback->setEnabled(val);
  action_Undo->setEnabled(val);
  //TODO: Redo not implemented yet, so keep disabled for now
}

void RGMainWindow::handleDrawModeChanged(bool activated)
{
  actionDraw_mode->setChecked(activated);
}

void RGMainWindow::handleVideoEncProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  delete mProcessWaitMessage;
  QByteArray output = mVideoEncProcess->readAllStandardOutput();
  QFile logFile(mVideoEncProcess->workingDirectory() + "/bmp2avi.log");
  if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    logFile.write(output);
    logFile.close();
  } else {
    QMessageBox::critical (this, "Error", "Unable to write bmp2avi.log! Disk full or no permissions?");
  }

  if (RGSettings::getDeleteBMPs()) {
    //Delete generated BMP's except for first and last frame (user might want to use them)
    for (int i = 1; i < mGeneratedBMPs.size() - 1; i++) {
      if (!QFile::remove(mGeneratedBMPs[i])) {
        QMessageBox::critical (this, "Error", "Unable to delete generated BMP's! No permissions?");
        break;
      }
    }
  }
  
  if (exitStatus == QProcess::NormalExit && exitCode == 0 && !output.contains("Error")) {
    QString outname  = RGSettings::getAviOutName();


    QString txt = QString(
      "<html>"
      "<center>"
      "<p>"
      "Your route has been generated in the selected directory."
      "Each frame is generated as a *.bmp file in that directory."
      "The name of the generated movie is <b>") + outname + QString(".avi</b>."
      "</p>"
      "</center>"
      "</html>"
    );
    QMessageBox::information (this, "Map Generation Finished", txt );
    
  } else {
    QMessageBox::critical (this, "Error", QString("Bmp2avi did not finish successfully! See file bmp2avi.log in output directory for details."));
  }

  mVideoEncProcess->deleteLater();
  blockUserInteraction(false);
}

void RGMainWindow::handleVideoEncProcessError(QProcess::ProcessError)
{
  QMessageBox::critical (this, "Error", "Bmp2avi execution failed!" );

  mVideoEncProcess->kill();

  if (mVideoEncProcess->state() != QProcess::NotRunning)
    QMessageBox::critical (this, "Error", "Unable to kill bmp2avi.exe, check your processes!" );

  mVideoEncProcess->deleteLater();
  blockUserInteraction(false);
}

void RGMainWindow::updateVehicleIcon(int index)
{
  QString iconFile = mIconCB->itemData(index).toString();

  mRGMapWidget->setVehicle(iconFile, mDrawMirroredIcon, mStartAngle);

  //Set preview vehicle, and we draw an extra west to east orientation arrow in it
  QPixmap pm = mRGMapWidget->getVehiclePixmap();
  if (pm.isNull()) {
    mVehiclePreviewLabel->setPixmap(pm);
  } else {
    QPixmap spm = pm.scaled(mVehiclePreviewLabel->width(),
                            mVehiclePreviewLabel->height(),
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation);
    QPainter painter(&spm);
    QPen pen(Qt::DashLine);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(0, spm.height() / 2, spm.width() - 1, spm.height() / 2);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLine(spm.width() - 1, spm.height() / 2, spm.width() - 4, spm.height() / 2 - 3);
    painter.drawLine(spm.width() - 1, spm.height() / 2, spm.width() - 4, spm.height() / 2 + 3);
    mVehiclePreviewLabel->setPixmap(spm);
  }
}

QIcon RGMainWindow::createIconForStyle(Qt::PenStyle style)
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

