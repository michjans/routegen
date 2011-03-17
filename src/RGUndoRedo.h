#ifndef RGUNDOREDO_H
#define RGUNDOREDO_H

#include <QObject>
#include <QVariant>
#include "RGGraphicsObjectUndo.h"

class RGUndoRedo : public QObject
{
    Q_OBJECT
public:
  explicit RGUndoRedo(QObject *parent = 0);
  void sendActionSignals();

signals:
  void undoPossible(bool);
  void redoPossible(bool);

public slots:
  void addUndo(RGGraphicsObjectUndo *,QVariant);
  void undo();
  void redo();

private:

  QList< QMap< RGGraphicsObjectUndo *,QVariant> >  mUndoList;
  QList< QMap< RGGraphicsObjectUndo *,QVariant> >  mRedoList;
  //RGRoute *mRoute;

};

#endif // RGUNDOREDO_H
