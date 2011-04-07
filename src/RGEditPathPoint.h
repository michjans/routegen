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

#ifndef RGEDITPATHPOINT_H
#define RGEDITPATHPOINT_H

#include <QGraphicsObject>
#include <QPoint>
#include <QtGui>

class RGEditPathPoint : public QGraphicsObject
{
  Q_OBJECT
public:
  explicit RGEditPathPoint(QGraphicsItem *parent,QPoint point);
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

signals:
  void editMovedPoint(bool);
  void editAddPoint(RGEditPathPoint *);

public slots:

protected:
  void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
  void mouseReleaseEvent(QGraphicsSceneMouseEvent * event );
  void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );

private:
  bool mMouseMove;

};


#endif // RGEDITPATHPOINT_H
