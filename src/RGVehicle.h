/*
    Copyright (C) 2008-2011  Michiel Jansen
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

#ifndef RGVEHICLE_H
#define RGVEHICLE_H


#include <QString>
#include <vector>
#include <QImage>
#include <QPixmap>


class RGVehicle
{
public:
  RGVehicle(const QString &filename="None",int size=0,bool mirror=false,int startAngle=0,int frameDelay=80);
  ~RGVehicle();
  int     getSize();
  int     getRawSize();
  bool    getMirror();
  int     getStartAngle();
  int     getDelay();
  QPixmap getPixmap();
  QPixmap getPixmapAtSize(int);
  QPixmap getPixmapAtAngle(int degrees);
  QString getName();

  void    setSize(int size);
  void    setMirror(bool mirror);
  void    setStartAngle(int selfAngle);


private:
  QImage  rotateImage(QImage &image, int degrees);
  QImage  scaleImage(QImage &image, int size);
  QImage  mirrorImage(QImage &image);
  void    createImages(int size,int angle,bool mirror);//create mImages from mRawImages

  QString             mFileName;
  bool                mMirror;
  int                 mStartAngle;
  int                 mSize;
  int                 mRawSize;
  int                 mFrameDelay;
  std::vector<QImage> mImages;
  std::vector<QImage> mRawImages;
};

#endif // RGVEHICLE_H
