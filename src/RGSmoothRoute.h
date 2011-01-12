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

#ifndef RGSMOOTHROUTE_H
#define RGSMOOTHROUTE_H

#include <QPainterPath>
#include <QList>
#include <QPoint>

class RGSmoothRoute
{
public:
  static QPainterPath SmoothRoute(QList<QPoint> RawRoute, int dsmooth=10);

private:
  static QPainterPath pathLineQuad(QPoint start,QPoint coef, QPoint end);
  static QPainterPath pathLineCubic(QPoint start,QPoint coef1,QPoint coef2, QPoint end);
  static QPoint getPointAtLength(QPoint start,QPoint end,int length);
};

#endif // RGSMOOTHROUTE_H
