#ifndef RGROUTE2_H
#define RGROUTE2_H

#include <QGraphicsItem>
#include "RGVehicleList.h"
#include "RGRouteUi.h"
#include "RGPath.h"
#include "RGEditPath.h"

class RGRoute2 : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit RGRoute2(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    QWidget *widgetSettings();
    void setSmoothCoef(int);
    void setEditMode(bool);
    void clearPath();
    int countFrames();
    void setCurrentFrame(int);
    void setIconlessBeginEndFrames(bool);
    void sceneRectChanged(const QRectF & rect);

signals:

private slots:
    void on_penChanged(const QPen & pen);
    void on_totalTimeChecked(bool);
    void on_smoothPathChecked(bool);
    void on_routeTimeChanged(int);
    void on_vehicleChanged(int);

private:
    QRectF mBoundingRect;
    RGRouteUi *mRouteUi;
    RGVehicleList     *mVehicleList;
    //RGVehicle *mVehicle;
    RGPath  *mPath;
    RGEditPath * mEditPath;
    bool              mIconlessBeginEndFrames;
};

#endif // RGROUTE2_H
