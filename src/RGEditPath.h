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
    void sceneRectChanged(const QRectF & rect);

protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
signals:
    void newPointList(QList<QPoint>);

public slots:
    void clear();
    void editPathPointMoved();
    void editPathPointAdd(RGEditPathPoint *);
    void editPathPointDel(RGEditPathPoint *);
private :
    void updatePointList();
private:
    QRectF mBoundingRect;
    QList<RGEditPathPoint*> mEditPathPointList;

};

#endif // RGEDITPATH_H
