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

#include <QDebug>
#include <QGeoRectangle>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPixmap>
#include <QProgressDialog>

namespace {
//Number of extra tiles to load round map, to give room for panning the map
const int marginTiles = 2;
}

RGOsmGraphicsView::RGOsmGraphicsView(QWidget* parent)
    : QGraphicsView(parent),
      mScene(new QGraphicsScene(this)),
      mRouteItem(nullptr),
      mZoomLevel(1)
{
    setScene(mScene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //We want the map to drag and zoom by actually loading the new OSM tiles, so we don't want to make
    //use of the default transformations provided by QGraphicsView and scene
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setRenderHint(QPainter::Antialiasing, false);
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //setInteractive(false);
    //setDragMode(QGraphicsView::NoDrag);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setCursor(Qt::OpenHandCursor);

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
    clearTiles();
    loadTiles();
}

void RGOsmGraphicsView::setGeoPath(const QGeoPath &geoPath)
{
    mGeoPath = geoPath;

    QGeoRectangle startGeoRect = mGeoPath.boundingGeoRectangle();

    //Load initial map location from incoming geo rectangle
    qDebug() << "startGeoRect:"
             << "  topLeft:" << startGeoRect.topLeft() << "  bottomLeft:" << startGeoRect.bottomLeft() << "  topRight:" << startGeoRect.topRight()
             << "  bottomRight:" << startGeoRect.bottomRight() << "  center:" << startGeoRect.center();

    mCenterCoord = startGeoRect.center();
    emit centerCoordChanged(mCenterCoord);
}

QPixmap RGOsmGraphicsView::renderMap()
{
    QRect fullMapRect(0, 0, mScene->width(), mScene->height());
    bool result = false;
    QPixmap outImage(mSize);
    QPainter painter(&outImage);
    //Don't draw the route on the generated map
    if (mRouteItem) mRouteItem->setVisible(false);

    //Force selected resolution
    //mScene->setSceneRect(mFixedSceneRect);
    QRectF targetRect(0.0, 0.0, mSize.width(), mSize.height());
    QPointF sceneCenter = mScene->sceneRect().center();
    QRectF sourceRect(sceneCenter.x() - mSize.width() / 2.0, sceneCenter.y() - mSize.height() / 2.0, mSize.width(), mSize.height());

    mScene->render(&painter, targetRect, sourceRect);
    painter.end();
    mOsmBackEnd.addAttribution(outImage);
    return outImage;
}

/*
void RGOsmGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mDragOrigin = event->pos();
        //qDebug() << "viewport size:" << viewport()->size();
        //qDebug() << "transformation:" << transform();
        //qDebug() << "Viewport center after centerOn:" << mapToScene(viewport()->rect().center());
        qDebug() << "mDragOrigin = " << mDragOrigin;
        setCursor(Qt::ClosedHandCursor); // Optional: better UX
    }
}

void RGOsmGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    auto delta = event->pos() - mDragOrigin;

    qDebug() << "delta:" << delta;
    qDebug() << "delta.x() / 256.0 = " << delta.x() / 256.0;

    mDragOrigin = event->pos();

    // Convert pixel movement to change in lat/lon
    qDebug() << "original mCenterCoord = " << mCenterCoord;

    QPointF centerTile = mOsmBackEnd.latLonToTile(mCenterCoord, mZoomLevel);
    centerTile.setX((centerTile.x() - delta.x()) / mOsmBackEnd.TILE_SIZE);
    centerTile.setY((centerTile.y() - delta.y()) / mOsmBackEnd.TILE_SIZE);
    mCenterCoord = mOsmBackEnd.tileToLatLon(centerTile, mZoomLevel);
    qDebug() << "shifted mCenterCoord = " << mCenterCoord;
    emit centerCoordChanged(mCenterCoord);

    event->accept();

    clearTiles();
    loadTiles();
}
*/

void RGOsmGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        //setCursor(Qt::OpenHandCursor);
        QPointF centerScene = mapToScene(viewport()->rect().center());
        QPointF centerTile = centerScene / mOsmBackEnd.TILE_SIZE;
        mCenterCoord = mOsmBackEnd.tileToLatLon(centerTile, mZoomLevel);
        qDebug() << "shifted mCenterCoord = " << mCenterCoord;
        clearTiles();
        loadTiles();
        emit centerCoordChanged(mCenterCoord);
        event->accept();
    }
}

void RGOsmGraphicsView::wheelEvent(QWheelEvent* event)
{
    clearTiles();
    int zoomDelta = event->angleDelta().y() > 0 ? 1 : -1;
    mZoomLevel = qBound(1, mZoomLevel + zoomDelta, 20);
    qDebug() << "zoom:" << mZoomLevel;
    emit zoomLevelChanged(mZoomLevel);
    loadTiles();
    viewport()->update();
}

void RGOsmGraphicsView::addTileToScene(const QImage& tile, int tileX, int tileY)
{
    qDebug() << "adding tile:" << tileX << ", " << tileY;
    // Calculate the tile's position in the scene
    int x = tileX * mOsmBackEnd.TILE_SIZE;
    int y = tileY * mOsmBackEnd.TILE_SIZE;

    // Create a pixmap item and add it to the scene
    QGraphicsPixmapItem* tileItem = new QGraphicsPixmapItem(QPixmap::fromImage(tile));
    tileItem->setPos(x, y);
    mScene->addItem(tileItem);

    updateProgressMonitor(tileX, tileY);

    //qDebug() << "scene rect is now: " << mScene->sceneRect();
}

void RGOsmGraphicsView::loadTiles()
{
    //The tilePos is a floating point position, which maps to the correct tile x,y when
    //rounding down to integers (i.e. one pixel in the tile represents 1/256 of the tile's size)
    QPointF tilePos = mOsmBackEnd.latLonToTile(mCenterCoord, mZoomLevel);
    qDebug() << "center tilePos is:" << tilePos;

    //Determine visible rectangle based on requested map resolution
    double centerX = tilePos.x() * mOsmBackEnd.TILE_SIZE;
    double centerY = tilePos.y() * mOsmBackEnd.TILE_SIZE;
    QPointF topLeft(centerX - mSize.width() / 2.0, centerY - mSize.height() / 2.0);
    mFixedSceneRect = QRectF(topLeft, mSize);

    //We add some margin around the scene so the user can scroll the map. Later on (when rendering the map)
    //we will pick the mFixedSceneRect again, that represents the chosen resolution for the map.
    QRectF sceneRect = mFixedSceneRect.marginsAdded(QMarginsF(mOsmBackEnd.TILE_SIZE * marginTiles,
                                                              mOsmBackEnd.TILE_SIZE * marginTiles,
                                                              mOsmBackEnd.TILE_SIZE * marginTiles,
                                                              mOsmBackEnd.TILE_SIZE * marginTiles));
    mScene->setSceneRect(sceneRect);

    // Calculate tile indices to load
    int beginX = std::floor(sceneRect.left() / mOsmBackEnd.TILE_SIZE);
    int endX = std::ceil(sceneRect.right() / mOsmBackEnd.TILE_SIZE) - 1;
    int beginY = std::floor(sceneRect.top() / mOsmBackEnd.TILE_SIZE);
    int endY = std::ceil(sceneRect.bottom() / mOsmBackEnd.TILE_SIZE) - 1;

    //We want to monitor if all tiles have been received
    initProgressMonitor(beginX, endX, beginY, endY);

    //Now request the actual tiles
    qDebug() << "Tile grid bounds:" << beginX << endX << beginY << endY;
    for (int x = beginX; x <= endX; ++x)
    {
        for (int y = beginY; y <= endY; ++y)
        {
            mOsmBackEnd.requestTile(x, y, mZoomLevel);
        }
    }

    // Center the view on the calculated tile position
    //qDebug() << "centerOn:" << centerX << "," << centerY;
    centerOn(centerX, centerY);
}

void RGOsmGraphicsView::clearTiles()
{
    mScene->clear();
    mRouteItem = nullptr;
}

void RGOsmGraphicsView::initProgressMonitor(int beginX, int endX, int beginY, int endY)
{
    setCursor(Qt::WaitCursor);
    qDebug() << "initProgressMonitor(" << beginX << "," << endX << "," << beginY << "," << endY;
    mTotalTilesRequested = 0;
    for (int x = beginX; x <= endX; ++x)
    {
        for (int y = beginY; y <= endY; ++y)
        {
            mPointMonitorMap.emplace(x,y);
        }
    }
    mTotalTilesRequested = mPointMonitorMap.size();
    qDebug() << "initProgressMonitor:mTotalTilesRequested=" << mTotalTilesRequested;
    emit initTileProgress(mTotalTilesRequested);
}

void RGOsmGraphicsView::updateProgressMonitor(int tileX, int tileY)
{
    qDebug() << "updateProgressMonitor(" << tileX << "," << tileY;
    QPoint tilePos(tileX, tileY);
    mPointMonitorMap.erase(tilePos);
    if (mPointMonitorMap.empty())
    {
        qDebug() << "updateProgressMonitor: all tiles received";
        drawGeoPath();
        unsetCursor();
        emit allTilesReceived();
    }
    else
    {
        qDebug() << "updateProgressMonitor:" << mTotalTilesRequested << "-" << mPointMonitorMap.size();
        emit tileProgress(mTotalTilesRequested - mPointMonitorMap.size());
    }
}

void RGOsmGraphicsView::drawGeoPath()
{
    if (mGeoPath.isEmpty()) return;

    // Create a QPainterPath from the geopoints
    QPainterPath path;
    bool firstPoint = true;
    for (const auto &geoPoint : mGeoPath.path())
    {
        QPointF point = mOsmBackEnd.latLonToWorld(geoPoint, mZoomLevel);
        if (firstPoint)
        {
            path.moveTo(point);
            firstPoint = false;
        }
        else
        {
            path.lineTo(point);
        }
    }

    // Add the path to the scene
    mRouteItem = mScene->addPath(path, QPen(Qt::blue, 4));
}
