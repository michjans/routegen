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


#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

#include "RGMainWindow.h"

/*
 * Version history:
 *  v1.0:   Initial version
 *  v1.1:   Added custom icons
 *          Added custom color selection for route
 *  v1.2:   Added rotating vehicle with direction of route (RGVehicle)
 *          Added animated vehicles (using animated gifs) (RGVehicle)
 *          When loading large maps, scrollbars appear (QScrollArea)
 *          Added spinbox for angle correction
 *          Added stop button for playback
 *  v1.2.1: Bugfix in initialization of route width
 *  v1.2.2: Added more error checking/logging for bmp2avi (bmp2avi.log in output directory).
 *  v1.2.3: Bugfix in file sequence number generation (5 digits instead of 3 should be enough)
 *  v1.3:   Route interpolation and variable route speed.
 *          Configuration settings (bmp2avi)
 *          Undo buffer
 *          Route style selection
 *  v1.3.1: Smooth curves experiment disabled (buggy) (see new RGRoute constructor)
 *          Added advanced tab to settings dialog to:
 *            -Enable the buggy smooth curves code (so the other RGRoute constructor is used)
 *            -Modify vehicle orientation parameters
 *          Automatically disable draw mode, when user clicks Preview or Generate route,
 *          when in interpolation mode, so the interpolation algorithm is triggered first.
 *          Show first route point as will after first click (interpolation mode)
 *  v1.4:   Added Google Maps importer
 *  v1.5:   Route generation improvements (smooth route, using bezier curves)
 *          Start new route button on toolbar
 *          Vehicle orientation and settings improved
 *          Generate iconless begin/end frames checkbox moved to preferences dialog
 *          Google maps importer fix (map scrollable and zoomable)
 *  v1.6    Route editable (selected points can be moved or deleted)
 *          Redo buffer
 *          Codec selection for video encoding under linux 
 *          Installation command for linux
 */

extern const QString applicationName("Route Generator version 1.6");

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  //For storing application settings
  app.setOrganizationName("MJProductions");
  app.setApplicationName("Route Generator");

  RGMainWindow *mainWindow = new RGMainWindow;
  mainWindow->setWindowTitle(applicationName);

  mainWindow->show();
  return app.exec();
} 

