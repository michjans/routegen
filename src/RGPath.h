#ifndef RGPATH_H
#define RGPATH_H

#include <QGraphicsObject>
#include <QPen>
#include <QtGui>

class RGPath : public QGraphicsObject
{
    Q_OBJECT
public:
    RGPath(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    int countFrames();
    int setCurrentFrame(int);
    QPointF getEndPos();
    float getAngle();

signals:

public slots:
    void newPointList(QList<QPoint>);
    void setDrawTime(int);
    void setSmoothPath(bool);
    void setSmoothCoef(int);
    void setTotalTime(int time);
    void setPlayMode(int playMode);
    void setFPS(int FPS);
    void setPen(const QPen & pen);


private:
    QPainterPath getPathAtStep(int step);
    QPainterPath getPathAtTime(int time);
    void createPath();
  void createSmoothPath();
  QPainterPath pathLineQuad(QPoint start,QPoint coef, QPoint end);
  QPainterPath pathLineCubic(QPoint start,QPoint coef1,QPoint coef2, QPoint end);
  QPoint getPointAtLength(QPoint start,QPoint end,int length);
  float getAngleAtTime(int time);
  float getAngleAtStep(int step);

private:
  QList<QPoint>     mRawPath;
  int               mTime;
  QPainterPath      mPath; //Processed final path
  QPainterPath      mPaintPath; //path that will be paint
  int               mTotalTime; //total time for interpolation
  int               mPlayMode; //set the mode for the video generation(0=stepbystep,1=TotalTimeSet,2=speedSet)
  int               mFPS;
  int               mDSmooth;
  bool              mSmoothPath;
  QPen              mPen;
  int               mCurrentFrame;
  QPointF            mEndPos;

};

#endif // RGPATH_H
