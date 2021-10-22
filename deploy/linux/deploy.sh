#!/bin/bash
#vehicles directory and google-maps-template.html should be placed inside ./routegen/usr/bin as well

mkdir -p ./routegen/usr/lib
mkdir -p ./routegen/usr/bin
./linuxdeployqt-continuous-x86_64.AppImage ./routegen/usr/share/applications/routegen.desktop -appimage -exclude-libs=libnss3.so,libnssutil3.so -extra-plugins=iconengines,platformthemes/libqgtk3.so,imageformats -no-translations
