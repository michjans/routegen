#ifndef RGVEHICLEORIGINPT_H
#define RGVEHICLEORIGINPT_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPointF>

#include <QtGui>

class RGVehicleOriginPt : public QGraphicsItem
{
public:
  RGVehicleOriginPt(QGraphicsItem *parent=0);
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

protected:
  void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
  void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

private:
};

#endif // RGVEHICLEORIGINPT_H
