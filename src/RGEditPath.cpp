#include "RGEditPath.h"

RGEditPath::RGEditPath(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    mBoundingRect(QRectF())
{
  setPos(0,0);
  setCursor(Qt::CrossCursor);
}

QRectF RGEditPath::boundingRect() const
{
  /*if(this->scene()!=0)
    return this->scene()->sceneRect();
  else return QRectF();*/
  qDebug()<<"mEditPath boundingRect :"<<mBoundingRect;
  return mBoundingRect;
 }

void RGEditPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  //painter->setBrush(Qt::black);
  //painter->drawRect(-10,-10,20,20);
}

void RGEditPath::sceneRectChanged(const QRectF & rect)
{
  prepareGeometryChange();
  mBoundingRect=rect;
}

void RGEditPath::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  RGEditPathPoint *testpoint=new RGEditPathPoint(this,event->pos().toPoint());
  mEditPathPointList.append(testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint()),this,SLOT(editPathPointMoved()));
  QObject::connect(testpoint,SIGNAL(editAddPoint(RGEditPathPoint *)),this,SLOT(editPathPointAdd(RGEditPathPoint *)));
  QObject::connect(testpoint,SIGNAL(editDelPoint(RGEditPathPoint *)),this,SLOT(editPathPointDel(RGEditPathPoint *)));
  updatePointList();
}

void RGEditPath::clear()
{
  for(int i=0;i<mEditPathPointList.size();++i){
    delete mEditPathPointList.at(i);
  }
  mEditPathPointList.clear();
  updatePointList();
}

void RGEditPath::editPathPointMoved()
{
  updatePointList();
}

void RGEditPath::editPathPointAdd(RGEditPathPoint * point)
{
  int pointId=mEditPathPointList.indexOf(point);
  QPoint newpoint=QPoint();
  if (pointId!=0) newpoint=(mEditPathPointList.at(pointId)->pos().toPoint()+mEditPathPointList.at(pointId-1)->pos().toPoint())/2;
  RGEditPathPoint *testpoint=new RGEditPathPoint(this,newpoint);
  mEditPathPointList.insert(pointId,testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint()),this,SLOT(editPathPointMoved()));
  QObject::connect(testpoint,SIGNAL(editAddPoint(RGEditPathPoint *)),this,SLOT(editPathPointAdd(RGEditPathPoint *)));
  QObject::connect(testpoint,SIGNAL(editDelPoint(RGEditPathPoint *)),this,SLOT(editPathPointDel(RGEditPathPoint *)));

  updatePointList();
}

void RGEditPath::editPathPointDel(RGEditPathPoint * point)
{
  //remove editPoint:
  mEditPathPointList.removeAll(point);
  delete point;
  updatePointList();
}

void RGEditPath::updatePointList()
{
  QList<QPoint> pointList;
  for(int i=0;i<mEditPathPointList.size();++i){
    pointList.append(mEditPathPointList.at(i)->pos().toPoint());
  }
  emit newPointList(pointList);
}
