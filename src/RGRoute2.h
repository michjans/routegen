#ifndef RGROUTE2_H
#define RGROUTE2_H

#include <QGraphicsItem>
#include "RGVehicleList.h"
#include "RGRouteUi.h"

class RGRoute2 : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit RGRoute2(QObject *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    QWidget *widgetSettings();

signals:

public slots:

private:
    RGRouteUi *mRouteUi;
    RGVehicleList     *mVehicleList;

};

#endif // RGROUTE2_H
