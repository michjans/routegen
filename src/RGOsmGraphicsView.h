/*
    Copyright (C) 2009-2024  Michiel Jansen

  This file is part of Route Generator.

    Route Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RGVOSMGRAPHICSVIEW_H
#define RGVOSMGRAPHICSVIEW_H

#include <QGeoCoordinate>
#include <QGeoPath>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QTimeLine>
#include <QTimer>

#include <set>

#include "RGOsmBackend.h"

class RGOsmGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RGOsmGraphicsView(QWidget* parent = 0);

    QSize sizeHint() const override;

    void setFixedSceneResolution(const QSize& res);

    void loadMap(const QGeoCoordinate& coord, int zoom, QSize size);

    void setGeoPath(const QGeoPath& geoPath);

    RGOsmBackend& osmBackend()
    {
        return mOsmBackEnd;
    }

    QPixmap renderMap();

signals:
    void zoomLevelChanged(int zoom);
    void centerCoordChanged(const QGeoCoordinate& centerCoord);

    //For progress monitoring of received tiles
    void initTileProgress(int totalTiles);
    void tileProgress(int progress);
    void allTilesReceived();

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void addTileToScene(const QImage& tile, int tileX, int tileY);

private:
    void loadTiles();
    void clearTiles();
    void initProgressMonitor(int beginX, int endX, int beginY, int endY);
    void updateProgressMonitor(int tileX, int tileY);
    void drawGeoPath();
    void drawTargetRect();

    //If the mPointMonitorMap contains points, we know that tiles are currently
    //being requested
    bool tileLoadingInProgress() const { return !mPointMonitorMap.empty(); }

    RGOsmBackend mOsmBackEnd;

    QGraphicsScene* mScene;
    QPointF mDragOrigin;
    QGeoCoordinate mCenterCoord;
    QRectF mCutoutRect;
    QGeoPath mGeoPath;
    QGraphicsPathItem *mRouteItem;
    QGraphicsRectItem *mCutoutRectItem;
    QSize mSize;
    int mZoomLevel;

    struct QPointLess {
        bool operator()(const QPoint& a, const QPoint& b) const {
            if (a.x() < b.x()) return true;
            if (a.x() > b.x()) return false;
            return a.y() < b.y();
        }
    };

    //For monitoring the number of requested tiles
    int mTotalTilesRequested;
    std::set<QPoint, QPointLess> mPointMonitorMap;
};

#endif // RGVOSMGRAPHICSVIEW_H
