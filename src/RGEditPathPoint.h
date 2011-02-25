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
    void editMovedPoint();
    void editAddPoint(RGEditPathPoint *);
    void editDelPoint(RGEditPathPoint *);
public slots:

protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    /*void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    //void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event );*/
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
private:

};


#endif // RGEDITPATHPOINT_H
