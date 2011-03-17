#include "RGEditPath.h"

RGEditPath::RGEditPath(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    mBoundingRect(QRectF()),
    mousePressed(false)
{
  setPos(0,0);
  setCursor(Qt::CrossCursor);
}

QRectF RGEditPath::boundingRect() const
{
  return mBoundingRect;
 }

void RGEditPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  Q_UNUSED(painter);
}

void RGEditPath::on_sceneRectChanged()
{
  prepareGeometryChange();
  mBoundingRect=this->scene()->sceneRect();
}

void RGEditPath::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event->button()==Qt::LeftButton){
    mousePressed=true;
    addPoint(event->pos().toPoint());
    updatePointList(false); //is not undoable
  }
}

void RGEditPath::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  //check if the left button have been pressed (event->button() does not work)
  if(mousePressed==false)
    return;
  addPoint(event->pos().toPoint());
  updatePointList(false); //is not undoable
}

void RGEditPath::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event->button()==Qt::LeftButton){
    mousePressed=false;
    updatePointList();
  }
}

void RGEditPath::clear(bool canUndo)
{
  for(int i=0;i<mEditPathPointList.size();++i){
    delete mEditPathPointList.at(i);
  }
  mEditPathPointList.clear();
  updatePointList(canUndo);
}

void RGEditPath::setNewPoints(QList<QPoint> pointlist)
{
  clear(false);
  for(int i=0;i<pointlist.size();i++){
    addPoint(pointlist.at(i));
  }
  updatePointList(false); //is not undoable
}

void RGEditPath::editPathPointMoved(bool canUndo)
{
  updatePointList(canUndo);
}

void RGEditPath::editPathPointAdd(RGEditPathPoint * point)
{
  int pointId=mEditPathPointList.indexOf(point);
  QPoint newpoint=QPoint();
  if (pointId!=0) newpoint=(mEditPathPointList.at(pointId)->pos().toPoint()+mEditPathPointList.at(pointId-1)->pos().toPoint())/2;
  RGEditPathPoint *testpoint=new RGEditPathPoint(this,newpoint);
  mEditPathPointList.insert(pointId,testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint(bool)),this,SLOT(editPathPointMoved(bool)));
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

void RGEditPath::updatePointList(bool canUndo)
{
  QList<QPoint> pointList;
  for(int i=0;i<mEditPathPointList.size();++i){
    pointList.append(mEditPathPointList.at(i)->pos().toPoint());
  }
  emit newPointList(pointList,canUndo);
}

void RGEditPath::addPoint(QPoint point)
{
  RGEditPathPoint *testpoint=new RGEditPathPoint(this,point);
  mEditPathPointList.append(testpoint);
  QObject::connect(testpoint,SIGNAL(editMovedPoint(bool)),this,SLOT(editPathPointMoved(bool)));
  QObject::connect(testpoint,SIGNAL(editAddPoint(RGEditPathPoint *)),this,SLOT(editPathPointAdd(RGEditPathPoint *)));
  QObject::connect(testpoint,SIGNAL(editDelPoint(RGEditPathPoint *)),this,SLOT(editPathPointDel(RGEditPathPoint *)));
}
