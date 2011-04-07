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

#include "RGUndoRedo.h"

RGUndoRedo::RGUndoRedo(QObject *parent) :
  QObject(parent)
{
}

void RGUndoRedo::addUndo(RGGraphicsObjectUndo * undoObject,QVariant data)
{
  QMap< RGGraphicsObjectUndo *,QVariant> map;
  map.insert(undoObject,data);
  mUndoList.append(map);
  if(mUndoList.size()>10)
    mUndoList.removeFirst();
  sendActionSignals();
}

void RGUndoRedo::undo()
{
  mRedoList.append(mUndoList.takeLast());
  undoredo();
}

void RGUndoRedo::redo()
{
  mUndoList.append(mRedoList.takeLast());
  undoredo();
}

void RGUndoRedo::undoredo()
{
  QMap< RGGraphicsObjectUndo *,QVariant> map=mUndoList.last();
  RGGraphicsObjectUndo * undoObject = map.keys().at(0);
  undoObject->undoredo(map.value(undoObject));
  sendActionSignals();
}

void RGUndoRedo::sendActionSignals()
{
  if(mUndoList.size()>=2)
    emit undoPossible(true);
  else
    emit undoPossible(false);
  if(mRedoList.size()>=1)
    emit redoPossible(true);
  else
    emit redoPossible(false);
}
