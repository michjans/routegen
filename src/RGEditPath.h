#ifndef RGEDITPATH_H
#define RGEDITPATH_H

#include <QGraphicsObject>
#include <QtGui>
#include "RGEditPathPoint.h"

class RGEditPath : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RGEditPath(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);


protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    void keyPressEvent ( QKeyEvent * event );
    void keyReleaseEvent ( QKeyEvent * event );

signals:
    void newPointList(QList<QPoint>,bool);

public slots:
    void clear(bool canUndo=true);
    void setNewPoints(QList<QPoint>);
    void editPathPointMoved(bool canUndo=true);
    void editPathPointAdd(RGEditPathPoint *);
    void editPathPointDel(RGEditPathPoint *);
    void on_sceneRectChanged();

private :
    void updatePointList(bool canUndo=true);
    void addPoint(QPoint);
    void deleteSelectedPoints();
private:
    QRectF mBoundingRect;
    QList<RGEditPathPoint*> mEditPathPointList;
    bool mMousePressed;
    bool mFreeDraw;
    bool mSelect;

};

#endif // RGEDITPATH_H
