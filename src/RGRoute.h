#ifndef RGROUTE_H
#define RGROUTE_H

#include <QGraphicsObject>
#include "RGVehicleList.h"
#include "RGRouteUi.h"
#include "RGPath.h"
#include "RGEditPath.h"

class RGRoute : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RGRoute(QGraphicsItem *parent = 0);
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
    void canGenerate(bool);

private slots:
    void on_penChanged(const QPen & pen);
    void on_totalTimeChecked(bool);
    void on_smoothPathChecked(bool);
    void on_routeTimeChanged(int);
    void on_vehicleChanged();
    void on_pathChanged(QList<QPoint>);

private:
    QRectF mBoundingRect;
    RGRouteUi *mRouteUi;
    RGVehicleList *mVehicleList;
    RGPath  *mPath;
    RGEditPath * mEditPath;
    bool mIconlessBeginEndFrames;
    bool mShowVehicle;
};

#endif // RGROUTE_H
