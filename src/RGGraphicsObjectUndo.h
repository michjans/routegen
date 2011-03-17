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
  virtual void undo(QVariant=0);

};

#endif // RGGRAPHICSOBJECTUNDO_H
