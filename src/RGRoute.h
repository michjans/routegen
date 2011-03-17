#ifndef RGROUTE_H
#define RGROUTE_H

#include "RGGraphicsObjectUndo.h"
#include "RGVehicleList.h"
#include "RGRouteUi.h"
#include "RGPath.h"
#include "RGEditPath.h"

class RGRoute : public RGGraphicsObjectUndo
{
    Q_OBJECT
public:
    explicit RGRoute(QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    QWidget *widgetSettings();
    void setSmoothCoef(int);
    void setEditMode(bool);
    int countFrames();
    void setCurrentFrame(int);
    void setIconlessBeginEndFrames(bool);
    void setNewPoints(QList<QPoint>);
    virtual void undo(QVariant=0);

signals:
    void canGenerate(bool);
    void sceneRectChanged();
    void newUndoable(RGGraphicsObjectUndo *,QVariant);

public slots:
    void on_playbackChanged(bool);
    void clearPath();

private slots:
    void on_penChanged(const QPen & pen);
    void on_totalTimeChecked(bool);
    void on_smoothPathChecked(bool);
    void on_routeTimeChanged(int);
    void on_vehicleChanged();
    void on_pathChanged(QList<QPoint>,bool);

private:
    void updateVehicle();

protected:
    RGRouteUi *mRouteUi;
    RGVehicleList *mVehicleList;
    RGPath  *mPath;
    RGEditPath * mEditPath;
    bool mIconlessBeginEndFrames;
    bool mPlayback;
    bool mEditMode;
};

#endif // RGROUTE_H
