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

signals:

private slots:
    void on_penChanged(const QPen & pen);
    void on_totalTimeChecked(bool);
    void on_smoothPathChecked(bool);
    void on_routeTimeChanged(int);
    void on_vehicleChanged(int);

private:
    RGRouteUi *mRouteUi;
    RGVehicleList     *mVehicleList;
    RGPath  *mPath;
    RGEditPath * mEditPath;
};

#endif // RGROUTE2_H
