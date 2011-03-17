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
