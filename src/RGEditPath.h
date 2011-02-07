#ifndef RGEDITPATH_H
#define RGEDITPATH_H

#include <QGraphicsItem>
#include <QtGui>
#include "RGEditPathPoint.h"

class RGEditPath : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit RGEditPath(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
signals:
    void newPointList(QList<QPoint>);

public slots:
    void editPathPointMoved();
    void editPathPointAdd(RGEditPathPoint *);
    void editPathPointDel(RGEditPathPoint *);
private :
    void updatePointList();
private:
    QList<RGEditPathPoint*> mEditPathPointList;

};

#endif // RGEDITPATH_H
