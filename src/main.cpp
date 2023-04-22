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
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTranslator>

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
 *          Increased maximum of duration of output movie
 *          Codec selection for video encoding under linux
 *          Installation command for linux
 *          Improvements of edit dialog for vehicle settings
 *          Vehicle orientation (yes/no) is now a setting
 *          Deleting files in directory when not empty
 *          Adding custom vehicles from vehicle settings dialog
 *  v1.7    Using ffmpeg codec on Windows
 *          URL format of new version of google maps now supported
 *          Line width and style saved again
 *          Option to add N seconds still frame before/after movie
 *  v1.7.1  FFMpeg default converter and provided with the installation
 *  v1.8    Ported to Qt 5
 *  v1.8.1  pixel format of output video's is now always set to yuv420p
 *  v1.9    GPX import
 *          Loading / saving projects
 *          Storing Geo coordinates with map
 *          Manually enter FFMpeg commandline options
 *          Maximize button of google maps
 *          Increased maximum resolution of google map to 8K
 *          Default output container for movies now configurable (in case of ffmpeg generator)
 *          Tried to set default output frame images to png (in case of ffmpeg generator), but found out that
 *          saving/loading an image in bmp is much faster (due to uncompressed format), so left it at bmp.
 *          Added map and route status indicators (icons) in status bar
 *          SVG format support for custom vehicles
 *  v1.9.1  Maximum size for custom vehicles increased
 *          Vehicle movement less jerky when route moves around direct north or south
 *          Fix when map boundaries cross the 180 degrees longitude boundary (e.g. around New Zealand)
 *          Now using google map's web mercator projection algorithm to convert lat,lon to x,y
 *          More options after importing route from GPX file
 *          Prevent zoomlevels with decimal numbers in Google map URL's
 *          Google maps dialog geometry saved and restored
 *  v1.10.0 Zooming and pan of map
 *          Possible to select map type during Google Maps import (roadmap, sattelite, terrain, etc.)
 *          GPX import now automatically zooms map to boundaries of route
 *          Create new empty folder when files detected, instead of deleting all files
 *          Fixed default location of ffmpeg on linux (removed /usr/bin)
 *          Added more default (animated) vehicles
 *          Vehicle rotation smoothness increased
 *  v1.11.0 GeoTiff support.
 *          Possibility to set fixed output resolution for output video's.
 *          Scrolling/sliding map with fixed resolution of output video when generating movie.
 *          Because background map resolution can now be higher than the output resolution, you can
 *          now specify a ratio in relation to the selected output resolution in the Google maps importer.
 *          Make it possible to select existing folder again.
 *          FPS setting now passed correctly to the ffmpeg commandline.
 *          Possibility to set different file type for generated video frames (e.g. PNG, JPG, etc.)
 */

extern const QString applicationName("Route Generator version 1.12.0-alpha");

int main(int argc, char* argv[])
{
    //TODO:qInstallMessageHandler(myMessageOutput);

    QApplication app(argc, argv);

    QTranslator translator;
    //if (translator.load(QLocale(), QLatin1String("routegen"), QLatin1String("_"), QLatin1String(":/i18n")))
    if (translator.load(QLatin1String("routegen_nl"), QLatin1String(":/i18n/i18n")))
    {
        QApplication::installTranslator(&translator);
    }

    //For storing application settings
    app.setOrganizationName("MJProductions");
    app.setApplicationName("Route Generator");

    RGMainWindow* mainWindow = new RGMainWindow;
    mainWindow->setWindowTitle(applicationName);

    mainWindow->show();
    return app.exec();
}
