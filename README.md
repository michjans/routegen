# Route Generator

Route Generator is a tool to draw routes on a map and generate a movie from it.
This movie can be imported in your video editing software, so you can add it to
your own movies.

I have cloned this repository from SourceForge, to have the source code available
on GitHub as well, with the intention that more people are willing to contribute
to it.
	  
## Installation
On Windows use the distributed installer named routegen-winxx-x.x.exe
On Linux and Mac OS the program has to be build from source code (see below)

## Building Route Generator from the source code
Since version x.x Route Generator should be built using Qt 5.15 or higher using CMake.
So Qt 5.15 or higher should be downloaded and installed, including CMake.
Also now Route Generator supports importing GeoTiff maps, so it requires
libgeotiff development libraries to be installed, before it can be build.
Libgeotiff depends on libtiff, libproj and sqlite3, so they have to be installed as well.

### Install libgeotiff and dependent libraries

#### Linux (Ubuntu or similar OS)

Execute the following commands:
- sudo apt install sqlite3
- sudo apt install proj
- sudo apt install libproj-dev
- mkdir libgeotiff
- cd libgeotiff
- git clone https://github.com/OSGeo/libgeotiff.git
- git checkout 1.7.1
- cd libgeotiff

Then follow the GeoTiff installation instructions and choose for the cmake installation,

so that the GeoTIFF package can be found from the CMakeLists.txt, e.g.
- cd build_cmake
- sudo make install

#### Windows

On Windows all dependent packages have to be build from source.
- Download libtiff from https://download.osgeo.org/libtiff/ (at least version 4.5)
- Download Proj (dev) from https://proj.org/download.html (at least version 9.1)
- Download sqlite3 source code from: https://sqlite.org/download.html (sqlite-amalgamation-3400100.zip)
- Download libgeotiff source code from https://github.com/OSGeo/libgeotiff/releases/tag/1.7.1

- Unzip the downloaded zip files in a development directory, e.g. C:\Users\mjans\dev
- Create a directory to deploy the locally installed libraries to (from now on C:\Users\mjans\dev\local is used as an example)
- mkdir C:\Users\mjans\dev\local
- Assuming Qt is already installed:
- Open Qt 5.15.2 MSVC 2019 Cmd shell, then execute:
- "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

- cd ...\tiff-4.5.0
- mkdir Release
- cd Release
- cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/Users/mjans/dev/local -G "NMake Makefiles" ..
- nmake install

- cd ...\sqlite-amalgamation-3400100
- cl -O2 -D_USRDLL /D_WINDLL /DSQLITE_ENABLE_FTS4 /DSQLITE_ENABLE_RTREE /DSQLITE_ENABLE_COLUMN_METADATA /DSQLITE_API=__declspec(dllexport)  sqlite3.c /MT /link /DLL /out:sqlite3.dll
- cl -O2 /DSQLITE_ENABLE_FTS4 /DSQLITE_ENABLE_RTREE /DSQLITE_ENABLE_COLUMN_METADATA shell.c sqlite3.c /MT /Fesqlite3.exe
- copy *.h files to C:/Users/mjans/dev/local/include (the local place where you want your libraries be installed)
- copy *.lib file to C:/Users/mjans/dev/local/lib
- copy *.dll file to C:/Users/mjans/dev/local/bin
- copy *.exe file to C:/Users/mjans/dev/local/bin

- cd ...\proj-9.1.0
- mkdir Release
- cd Release
- cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DBUILD_PROJSYNC=OFF -DCMAKE_INSTALL_PREFIX=C:/Users/mjans/dev/local -DENABLE_CURL=OFF -G "NMake Makefiles" ..
- nmake install

- cd ...\libgeotiff-1.7.1
- mkdir Release
- cd Release
- cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="/WX" -DCMAKE_CXX_FLAGS="/WX" -DCMAKE_INSTALL_PREFIX=C:/Users/mjans/dev/local -DPROJ_INCLUDE_DIR=C:/Users/mjans/dev/local/include -DPROJ_LIBRARY=C:/Users/mjans/dev/local/lib/proj.lib -G "NMake Makefiles" ..
- cmake --build . --config Release --target install

### Build Route Generator itself
After everything is setup correctly, now Route Generator itself can be build.
- unzip or clone the Route Generator source code in a new directory

#### Windows
- Open Qt 5.15.2 MSVC 2019 Cmd shell, then execute:
- "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
- cd to the the directory where you unzipped or cloned the source code
  (e.g. cd .../routegen/src)
- mkdir Release
- cd Release
- cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/Users/mjans/dev/local ..
- nmake

Note: when using QtCreator on Windows you also have to set the CMAKE_INSTALL_PREFIX key in the CMake
      build setting to point to the "C:/Users/mjans/dev/local", directory, otherwise GeoTIFF cannot be found.

#### Linux (or Mac)
- cd to the the directory where you unzipped or cloned the source code
  (e.g. cd routegen/src)
- mkdir Release
- cd Release
- cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=. ..

Builds release version of routegen executable locally
- make

Will install the release executable in the bin sub-directory
Note that this step is required to create a release executable that has the correct
rpath set to ${ORIGIN}/lib, to be able to distribute the Qt libraries in a lib
sub-directory (defined in qt.conf)!
- make install 

## Managing the translations
Some information regarding translation of Route Generator (up to now only, EN, NL and IT supported)

To generate or update the translation source files, I now manually execute lupdate/lrelease, because
doing this automatic during the build process has some disadvantages. Also it is not required to re-update
the ts files each time. This is only required when new dialogs or (translatable) texts are added.
To regenerate the ts files (i.e. parse the source code to search for new translatable strings:

- cd src
- lupdate *.cpp *.ui -locations absolute -ts i18n/routegen_en.ts i18n/routegen_nl.ts i18n/routegen_it.ts

After this step the ts files can be opened in Qt linguist and translations can be added.

Then to generate the qm files that are used runtime (and should also be deployed):
- lrelease i18n/routegen_nl.ts i18n/routegen_it.ts

Additionally the default Qt translations (e.g. default Ok, Cancel, Yes, No buttons) should
also be available (if available for the translated language), so we (at least) need to deploy
a single translation file for the used Qt base packages.

- cp /home/mjansen/Qt/5.15.2/gcc_64/translations/qtbase_it.qm i18n.qtbase_it.qm

Finally we embed all required qm files in the routegen.qrc file and reference them from there.

## Version history
- 1.0   -Initial version
- 1.1   -Added custom vehicle icons by adding icons (image files, e.g. *.png,
         *.jpg, etc.) to the vehicles sub-folder of routegen.
        -Added custom color selection for route path.
- 1.2   -Vehicle icon now rotates with route direction
        -Added vehicle angle correction spinbox and preview
        -Animated vehicle icons
        -Showing scrollbars for large input maps
        -Added stop button while playing back
- 1.2.1 -Bugfix in route width initialization (not released on SourceForge)
- 1.2.2 -Error checking and logging of bmp2avi execution (bmp2avi.log)
- 1.2.3 -Bugfix in frame naming, causing routes over 1000 frames to be added
         in wrong order in AVI file (during bmp2avi conversion)
- 1.3   -Route interpolation and variable route speed.
        -Preferences (bmp2avi)
        -Undo buffer
        -Route style selection
- 1.3.1 -Smooth curves experiment disabled (buggy) (see new RGRoute constructor)
        -Added advanced tab to settings dialog to:
          -Enable the buggy smooth curves code
          -Change radius of curves
          -Modify vehicle orientation parameters
        -Automatically disable draw mode, when user clicks Preview or
         Generate route.
        -Show first route point as will after first click (interpolation mode)
- 1.4   -Import from Google Maps
- 1.5   -Route generation improvements (smooth route, using bezier curves)
        -Start new route button on toolbar
        -Vehicle orientation and settings improved
        -Generate iconless begin/end frames checkbox moved to preferences dialog
        -Google maps importer fix (map scrollable and zoomable)
- 1.6   -Route editable (selected points can be moved or deleted)
        -Redo buffer
        -Codec selection for video encoding under linux 
        -Installation command for linux
        -Improvements of edit dialog for vehicle settings
        -Vehicle orientation (yes/no) is now a setting
        -Deleting files in directory when not empty (when generating movie)
        -Adding custom vehicles from vehicle settings dialog
- 1.7   -Using ffmpeg codec on Windows
        -URL format of new version of google maps now supported
        -Line width and style saved again
        -Option to add N seconds still frame before/after movie
- 1.7.1 -FFmpeg for Windows is now distributed with the Windows version of
         Route Generator and will be selected as the default at installation.
         The Zeranoe FFmpeg Windows builds are provided by Kyle Schwarz from:
		 http://ffmpeg.zeranoe.com/builds/
		 NOTE: To safe space the executables ffplay.exe and ffprobe.exe are
               removed from the distribution.
- 1.8  -Route Generator ported to Qt 5.
       -No main functional changes, however import from Google maps now works again
       -Updated Zeranoe FFmpeg to 4.1.3
- 1.8.1 -Pixel format always yuv420p (when supported by selected codec)	   -
- 1.9 release
  - GPX Import (both routes and tracks)
  - Map and route can now be saved and loaded from project files
  - Storing Geo coordinates with map and route
  - Manually enter FFMpeg commandline options
  - Maximize button of google maps
  - Increased maximum resolution of google map to 8K
  - Default output format of video files now configurable for ffmpeg (default still avi)
  - Added map and route status indicators (icons) in status bar
  - SVG format support for custom vehicles
- 1.9.1 release
  - Maximum size for custom vehicles increased
  - Vehicle movement less jerky when route moves around direct north or south
  - Fix when map boundaries cross the 180 degrees longitude boundary
  - Lat/lon to x,y using Google maps (web mercator) projection algorithm
  - More options after importing route from GPX file
  - Prevent zoomlevels with decimal numbers in Google map URL's
  - Google maps dialog geometry saved and restored
- 1.10.0 release
  - Panning and zooming of map using mouse (and wheel)
  - Possible to select map type during Google Maps import (roadmap, sattelite, terrain, etc.)
  - GPX import now automatically zooms map to boundaries of route
  - Always create new empty folder when files are detected (to prevent that
    files are lost coincidentally)
  - Fixed default location of ffmpeg on linux (removed /usr/bin); assume it
    can always be found in the PATH
  - Added more default (animated) vehicles (source: gifsanimes.com)
- 1.11.0 release
  - GeoTiff support.
  - Possibility to set fixed output resolution for output video's.
  - Scrolling/sliding map with fixed resolution of output video when generating movie.
  - Because background map resolution can now be higher than the output resolution, you can
    now specify a ratio in relation to the selected output resolution in the Google maps importer.
  - Make it possible to select existing folder again.
  - FPS setting now passed correctly to the ffmpeg commandline.
  - Possibility to set different file type for generated video frames (e.g. PNG, JPG, etc.)
 
## Technical details

Route Generator is developed using the GPL version of Qt 5.15
(Copyright (C) 2008-2021  The Qt Company Ltd. All rights reserved).
Qt can be [downloaded](https://www.qt.io/download) from The Qt Company website.


On Linux, Route Generator makes use of a 3rdparty tool:
FFmpeg licensed under the LGPLv2.1.
This tool is not included in the source, so you will have to install it on
your computer before running Route Generator. Make sure it is installed if you
want to be able to generate a movie.

FFmpeg for Windows is also distributed with Route Generator for Windows
and will be selected as the default at installation.
The CODEX FFmpeg Windows builds are provided by Gyan Doshi from:
https://www.gyan.dev/ffmpeg/builds/

On Windows, Route Generator can also make use of a freeware 3rdparty tool:
Bmp2Avi (Copyright (C) Paul Roberts 1996 - 1998)
This tool is available in the subdirectory bmp2avi. By default Route Generator
tries to find bmp2avi.exe in that directory, so you can leave it there.
Route Generator will automatically check for bmp2avi.exe in this directory
when it starts up. If it cannot find it in the default location, it will ask
you to browse to a different location.
Bmp2Avi can freely be re-distributed as long as you provide the documentation
with it (also located in the bmp2avi sub-directory).

## Availability
Route Generator is build for Windows and Linux by default.
However, it's developed using the cross-platform GUI toolkit, Qt,
which is also available for other Mac. So it is also possible
to build and run Route Generator on Mac as well, but you have to build it
yourself from the source code (see building instructions below).
NOTE: Route Generator makes use of a video encoder (Bmp2Avi on Windows and 
ffmpeg on linux)! However, the video encoder is not required to run Route
Generator. The only thing that will not work without the video encoder is the
last stage of the generation process:converting a list of bmp files to an avi
file.

## License
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

For more details about GPL see the LICENSE file 

## Contact
If you have any more questions about or problems with using and or building
Route Generator, you can contact me at: info@routegenerator.net
Of course, suggestions for improvement are also welcome!

Michiel Jansen
