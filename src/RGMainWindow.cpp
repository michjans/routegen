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


#include <QtWidgets>

#include "RGMainWindow.h"
#include "RGSettings.h"
#include "RGSettingsDialog.h"
#include "RGGoogleMap.h"
#include "RGVehicleList.h"
#include "RGEncVideo.h"
#include "RGEncFFmpeg.h"
#include "RGEncBmp2avi.h"
#include "RGRoute.h"
#include "RGRouteUi.h"
#include "RGViewWidget.h"
#include "RGUndoRedo.h"

#include "ui_routegen.h"

//Defined in main.cpp
extern const QString applicationName;

RGMainWindow::RGMainWindow(QWidget *parent)
  :QMainWindow(parent),
     mVideoEncoder(nullptr)
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
  action_Redo = ui.action_Redo;
  actionGenerate_map = ui.actionGenerate_map;
  actionPlayback = ui.actionPlayback;
  actionStop = ui.actionStop;

  action_Undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  action_Redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

  actionStop->setEnabled(false);
  actionGenerate_map->setEnabled(false);
  actionPlayback->setEnabled(false);
  actionSave_image->setEnabled(false);
  actionDraw_mode->setEnabled(false);
  actionNew_route->setEnabled(false);
  action_Undo->setEnabled(false);
  action_Redo->setEnabled(false);

  //Video Encoder:
	initVideoEncoderFromSettings();

  //ViewWidget :
  mView = new RGViewWidget();
  ui.centralwidget->layout()->addWidget(mView);
  QObject::connect(actionStop, SIGNAL(triggered()),
                   mView, SLOT(stop()));
  QObject::connect(mView, SIGNAL(playbackStopped(bool)),
                   this, SLOT(enableGenerateActions(bool)));

  //Route :
  mRoute= new RGRoute();
  mRoute->setZValue(1);
  mRoute->setSmoothCoef(RGSettings::getSmoothLength());
  mRoute->setIconlessBeginEndFrames(RGSettings::getIconLessBeginEndFrames());
  mView->addRoute(mRoute);
  QObject::connect(mRoute, SIGNAL(canGenerate(bool)),
                   this, SLOT(enableGenerateActions(bool)));

  //Route UI (Toolbar with route controls):
  mRouteUi = new RGRouteUi();
  QObject::connect(mRouteUi,SIGNAL(penChanged(const QPen &)),mRoute,SLOT(changePen(const QPen &)));
  QObject::connect(mRouteUi,SIGNAL(smoothPathChecked(bool)),mRoute,SLOT(activateSmoothPath(bool)));
  QObject::connect(mRouteUi,SIGNAL(totalTimeChecked(bool)),mRoute,SLOT(activateTotalTime(bool)));
  QObject::connect(mRouteUi,SIGNAL(routeTimeChanged(int)),mRoute,SLOT(setRouteTime(int)));
  QObject::connect(mRouteUi,SIGNAL(vehicleChanged()),mRoute,SLOT(handleVehicleChange()));
  mRouteUi->setVehicleList(mRoute->getVehicleList());

  //set initial by sending signals
  mRouteUi->init();

  ui.mainLayout->insertWidget(0,mRouteUi);

  //Undo/Redo:
  mUndoRedo = new RGUndoRedo();
  QObject::connect(mRoute, SIGNAL(newUndoable(RGGraphicsObjectUndo *,QVariant)),
                   mUndoRedo, SLOT(addUndo(RGGraphicsObjectUndo *,QVariant)));
  QObject::connect(mUndoRedo, SIGNAL(undoPossible(bool)),
                   action_Undo, SLOT(setEnabled(bool)));
  QObject::connect(action_Undo, SIGNAL(triggered()),
                   mUndoRedo, SLOT(undo()));
  QObject::connect(mUndoRedo, SIGNAL(redoPossible(bool)),
                   action_Redo, SLOT(setEnabled(bool)));
  QObject::connect(action_Redo, SIGNAL(triggered()),
                   mUndoRedo, SLOT(redo()));

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
    mView->saveRenderedImage(fileName);
    RGSettings::setLastSaveDir(fileName);
  }
}

void RGMainWindow::on_actionPreferences_triggered(bool)
{
  RGSettingsDialog rgsettings(mVideoEncoder);

  if(rgsettings.exec()==QDialog::Accepted){
    mVideoEncoder->saveInSettings();
    mRoute->setSmoothCoef(rgsettings.getSmoothCoef());
    mRoute->setIconlessBeginEndFrames(rgsettings.getIconlessBeginEndFrames());

		if (RGSettings::getVideoEncoder() != mVideoEncoder->encoderName())
		{
			initVideoEncoderFromSettings();
		}
  }
  else
	{
    mVideoEncoder->updateFromSettings();
	}
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
    mView->loadImage(map);
    actionSave_image->setEnabled(true);
    actionDraw_mode->setEnabled(true);
    RGSettings::setLastOpenDir(fileName);
  }
}

void RGMainWindow::on_actionDraw_mode_triggered(bool checked)
{
  mView->stop();
  mRoute->setEditMode(checked);
  if(checked)
    this->statusBar()->showMessage("hold SHIFT to record free drawing, CTRL to select several points. Del key to delete selected points");
  else
    this->statusBar()->clearMessage();
}

void RGMainWindow::on_actionNew_route_triggered(bool)
{
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
  mRouteUi->blockEssentialControls(true);
  actionStop->setEnabled(true);
  action_Undo->setEnabled(false);
  action_Redo->setEnabled(false);
}

void RGMainWindow::on_actionStop_triggered(bool checked)
{
  Q_UNUSED(checked);
  mView->stop();
}

void RGMainWindow::on_actionGenerate_map_triggered(bool checked)
{
  Q_UNUSED(checked);
  bool generateBMPOK = false;
  QString lastGenDir = RGSettings::getLastGenDir();

  //Calculate MB the full movie (all uncompressed BMP's + AVI) will take
  //(This is a rough proximation, but the calculated number is always higher,
  // so we're safe)
  int sizeEstimate = int (mRoute->countFrames() * mRoute->scene()->sceneRect().width()* mRoute->scene()->sceneRect().height() *24/8/1024.0/1000.0* 2);
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
    //Directory should be empty
    QFileInfoList entries = qDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    if (entries.count() > 0)
    {
       QMessageBox::StandardButton answer = QMessageBox::question (this, "Folder not empty",
                                "Folder not empty, delete all existing files in this folder first?",
                                           QMessageBox::Yes | QMessageBox::No |
                                           QMessageBox::Cancel, QMessageBox::No);
       if (answer == QMessageBox::Yes)
       {
         //Delete all files (or at least try to)
         for (QFileInfoList::iterator it = entries.begin(); it != entries.end(); ++it)
         {
           if (it->isWritable())
           {
             qDebug() << "Deleting " + it->absoluteFilePath();
             QFile::remove(it->absoluteFilePath());
           }
         }
       }
       else if (answer == QMessageBox::Cancel)
       {
         return;
       }
       //Continue
    }

    RGSettings::setLastGenDir(dir);
    //generate images :
    mRoute->setEditMode(false);
    generateBMPOK = mView->generateMovie(dir, QString("map"), mGeneratedBMPs);
    mRoute->setEditMode(actionDraw_mode->isChecked());

    if (generateBMPOK) {
            if (mVideoEncoder != nullptr && mVideoEncoder->exists())
			{
				blockUserInteraction(true);
				QObject::connect(mVideoEncoder,SIGNAL(movieGenerationFinished()),this,SLOT(movieGenerationFinished()));
				mVideoEncoder->generateMovie(dir, QString("map"));
			}
			else
			{
				QMessageBox::warning (this, "Encoder unavailable", "No encoder available, only BMP's have been generated!");
			}
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
        "<p><b>") + applicationName + QString(" Copyright (C) 2008-2019  Michiel Jansen </b></p>"
                                              "<p>This program comes with ABSOLUTELY NO WARRANTY</p>"
                                              "This is free software, and you are welcome to redistribute it "
                                              "under certain conditions; see LICENSE file for details.</p>"
                                              "<p>This program was developed using the GPL version of Qt 5.12<br>"
                                              "(Copyright (C) 2008-2019  The Qt Company Ltd. All rights reserved),<br>"
                                              "Qt can be <a href=\"https://www.qt.io/download\"> downloaded </a>"
                                              "from the <a href=\"https://www.qt.io\">Qt</a> website. </p>"
                                              "<p>The conversion from BMP to AVI on Windows is provided by:<br>"
																							"<i>FFmpeg win32 static build by Kyle Schwarz.<br>"
																							"See: <a href=\"http://ffmpeg.zeranoe.com/builds/\">Zeranoe's FFmpeg Builds Home Page</a></i><br>"
																							"OR<br>"
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

  actionOpen_image->setEnabled(!busy);
  action_Quit->setEnabled(!busy);
  actionSave_image->setEnabled(!busy);
  actionDraw_mode->setEnabled(!busy);
  actionNew_route->setEnabled(!busy);
  actionGenerate_map->setEnabled(!busy);
  actionPlayback->setEnabled(!busy);
  actionImport_Google_Map->setEnabled(!busy);
  mRouteUi->blockEssentialControls(busy);
  //Stop is only enabled while playing back
  actionStop->setEnabled(false);
}

void RGMainWindow::enableGenerateActions(bool val)
{
  actionGenerate_map->setEnabled(val);
  actionPlayback->setEnabled(val);
  actionStop->setEnabled(false);
  mUndoRedo->sendActionSignals();

  //FIXME:
  //Route UI contains no "generate actions", so this is a hack to re-enable the controls in the route ui (to modify time, etc.),
  //when playback is finished (then val is true). In other cases (val == false) the controls should not be affected!
  //Currently the playback signal (triggering this slot) is emited by the view widget, but should be emitted by RGRoute
  if (val)
	  mRouteUi->blockEssentialControls(false);
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

void RGMainWindow::initVideoEncoderFromSettings()
{
    if ( mVideoEncoder != nullptr)
	{
		delete mVideoEncoder;
	}

#ifdef Q_OS_WIN
	QString vidEnc = RGSettings::getVideoEncoder();
	if (vidEnc == "bmp2avi")
	{
		mVideoEncoder = new RGEncBmp2avi();
	}
	else if (vidEnc == "ffmpeg")
	{
		mVideoEncoder = new RGEncFFmpeg();
	}
	else
	{
		QMessageBox::critical (this, "Error", tr("Unknown video encoder:") + vidEnc);
	}
#else
  mVideoEncoder = new RGEncFFmpeg();
#endif

	mVideoEncoder->initCodecExecutable();
  if(mVideoEncoder->exists())
	{
    qDebug()<<"encoder found !";
	}
}
