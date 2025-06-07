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
#include <QGeoRectangle>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QTimeLine>
#include <QTimer>

#include "RGOsmBackend.h"

class RGOsmGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit RGOsmGraphicsView(QWidget* parent = 0);

    QSize sizeHint() const override;

    void setFixedSceneResolution(const QSize& res);

    void loadMap(const QGeoCoordinate& coord, int zoom, QSize size);

    void setGeoRect(const QGeoRectangle& rect)
    {
        mGeoRect = rect;
    }

    RGOsmBackend& osmBackend()
    {
        return mOsmBackEnd;
    }

    QPixmap renderMap();

signals:
    void zoomLevelChanged(int zoom);
    void centerCoordChanged(const QGeoCoordinate& centerCoord);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void addTileToScene(const QImage& tile, int tileX, int tileY);

private:
    void loadTiles();
    void clearTiles();

    RGOsmBackend mOsmBackEnd;

    QGraphicsScene* mScene;
    QPointF mDragOrigin;
    QGeoCoordinate mCenterCoord;
    QGeoRectangle mGeoRect;
    QSize mSize;
    int mZoomLevel;
};

#endif // RGVOSMGRAPHICSVIEW_H
