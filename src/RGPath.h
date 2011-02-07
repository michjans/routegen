#ifndef RGPATH_H
#define RGPATH_H

#include <QGraphicsItem>
#include <QPen>
#include <QtGui>

class RGPath : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    RGPath(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

signals:

public slots:
    void newPointList(QList<QPoint>);
    void setDrawTime(int);
    void setIconlessBeginEndFrames(bool);
    void setSmoothPath(bool);
    void setSmoothCoef(int);
    void setTotalTime(int time);
    void setPlayMode(int playMode);
    void setFPS(int FPS);
    void setPen(const QPen & pen);

private:
  void createPath();

private:
  QList<QPoint>     mRawPath;
  int               mTime;
  QPainterPath      mPath; //Processed final path
  int               mTotalTime; //total time for interpolation
  int               mPlayMode; //set the mode for the video generation(0=stepbystep,1=TotalTimeSet,2=speedSet)
  int               mFPS;
  bool              mIconlessBeginEndFrames;
  int               mDSmooth;
  bool              mSmoothPath;
  QPen              mPen;

};

#endif // RGPATH_H
