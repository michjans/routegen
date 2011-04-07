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

#ifndef RGGRAPHICSOBJECTUNDO_H
#define RGGRAPHICSOBJECTUNDO_H

#include <QGraphicsObject>
#include <QVariant>

class RGGraphicsObjectUndo : public QGraphicsObject
{
  Q_OBJECT
public:
  explicit RGGraphicsObjectUndo(QGraphicsItem *parent = 0);

signals:

public slots:
  virtual void undoredo(QVariant=0);

};

#endif // RGGRAPHICSOBJECTUNDO_H
