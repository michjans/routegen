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

#include "RGOsmGraphicsView.h"
#include "RGOSMapProjection.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QProgressDialog>

RGOsmGraphicsView::RGOsmGraphicsView(QWidget* parent)
    : QGraphicsView(parent),
      mScene(new QGraphicsScene(this)),
      mZoomLevel(1)
{
    this->setScene(mScene);
    this->setRenderHint(QPainter::Antialiasing);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    this->setDragMode(QGraphicsView::ScrollHandDrag);

    connect(&mOsmBackEnd, &RGOsmBackend::tileAvailable, this, &RGOsmGraphicsView::addTileToScene);
}

QSize RGOsmGraphicsView::sizeHint() const
{
    return QSize(mScene->width(), mScene->height());
}

void RGOsmGraphicsView::setFixedSceneResolution(const QSize& res)
{
    mSize = res;
    if (mCenterCoord.isValid())
    {
        //A scene is loaded, re-load map
        clearTiles();
        loadTiles();
    }
}

void RGOsmGraphicsView::loadMap(const QGeoCoordinate& coord, int zoom, QSize size)
{
    mZoomLevel = zoom;
    mCenterCoord = coord;
    mSize = size;
    clearTiles();
    loadTiles();
}

QPixmap RGOsmGraphicsView::renderMap()
{
    QRect fullMapRect(0, 0, mScene->width(), mScene->height());
    bool result = false;
    QPixmap outImage(fullMapRect.size());
    QPainter painter(&outImage);
    mScene->render(&painter);
    return outImage;
}

void RGOsmGraphicsView::mousePressEvent(QMouseEvent* event)
{
    mDragOrigin = event->pos();
    qDebug() << "viewport size:" << viewport()->size();
    //qDebug() << "transformation:" << transform();
    qDebug() << "Viewport center after centerOn:" << mapToScene(viewport()->rect().center());
}

void RGOsmGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    auto delta = event->pos() - mDragOrigin;
    qDebug() << "delta:" << delta;
    translate(delta.x(), delta.y());
    mDragOrigin = event->pos();
}

void RGOsmGraphicsView::wheelEvent(QWheelEvent* event)
{
    clearTiles();
    int zoomDelta = event->angleDelta().y() > 0 ? 1 : -1;
    mZoomLevel = qBound(1, mZoomLevel + zoomDelta, 20);
    qDebug() << "zoom:" << mZoomLevel;
    loadTiles();
    //viewport()->update();
}

void RGOsmGraphicsView::addTileToScene(const QImage& tile, int tileX, int tileY)
{
    // Calculate the tile's position in the scene
    int x = tileX * RGOSMapProjection::TILE_SIZE;
    int y = tileY * RGOSMapProjection::TILE_SIZE;

    // Create a pixmap item and add it to the scene
    qDebug() << "adding tile item:" << x << ", " << y;
    QGraphicsPixmapItem* tileItem = new QGraphicsPixmapItem(QPixmap::fromImage(tile));
    tileItem->setPos(x, y);
    mScene->addItem(tileItem);

    qDebug() << "scene rect is now: " << mScene->sceneRect();
}

void RGOsmGraphicsView::loadTiles()
{
    //The tilePos is a floating point position, which maps to the correct tile x,y when
    //rounding down to integers (i.e. one pixel in the tile represents 1/256 of the tile's size)
    QPointF tilePos = RGOSMapProjection::latLonToTile(mCenterCoord, mZoomLevel);
    qDebug() << "center tilePos is:" << tilePos;

    //Determine visible rectangle based on requested map resolution
    double centerX = tilePos.x() * RGOSMapProjection::TILE_SIZE;
    double centerY = tilePos.y() * RGOSMapProjection::TILE_SIZE;
    QPointF topLeft(centerX - mSize.width() / 2.0, centerY - mSize.height() / 2.0);
    QRectF fixedSceneRect(topLeft, mSize);
    mScene->setSceneRect(QRectF());
    mScene->addRect(fixedSceneRect, QPen(Qt::red));

    // Calculate tile indices to load
    int beginX = std::floor(fixedSceneRect.left() / RGOSMapProjection::TILE_SIZE);
    int endX = std::ceil(fixedSceneRect.right() / RGOSMapProjection::TILE_SIZE) - 1;
    int beginY = std::floor(fixedSceneRect.top() / RGOSMapProjection::TILE_SIZE);
    int endY = std::ceil(fixedSceneRect.bottom() / RGOSMapProjection::TILE_SIZE) - 1;

    qDebug() << "Tile grid bounds:" << beginX << endX << beginY << endY;

    for (int x = beginX; x <= endX; ++x)
    {
        for (int y = beginY; y <= endY; ++y)
        {
            mOsmBackEnd.requestTile(x, y, mZoomLevel);
        }
    }

    //TODO: Draw mGeoRect on top if all tiles have been loaded

    // Center the view on the calculated tile position
    qDebug() << "centerOn:" << centerX << "," << centerY;
    centerOn(centerX, centerY);
}

void RGOsmGraphicsView::clearTiles()
{
    mScene->clear();
}
