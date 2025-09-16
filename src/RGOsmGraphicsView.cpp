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
      mCutoutRectItem(nullptr),
      mZoomLevel(1)
{
    setScene(mScene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //We want the map to drag and zoom by actually loading the new OSM tiles, so we don't want to completely
    //make use of the default transformations provided by QGraphicsView and scene
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setRenderHint(QPainter::SmoothPixmapTransform, false);
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
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
    if (tileLoadingInProgress()) return;
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

    //Try to calculate the optimal zoom level to contain the full route
    QGeoCoordinate topLeft = startGeoRect.topLeft();
    QGeoCoordinate bottomRight = startGeoRect.bottomRight();
    for (mZoomLevel = 18; mZoomLevel >= 0; --mZoomLevel) {
        QPointF p1 = mOsmBackEnd.latLonToWorld(topLeft, mZoomLevel);
        QPointF p2 = mOsmBackEnd.latLonToWorld(bottomRight, mZoomLevel);

        double bboxWidth  = fabs(p2.x() - p1.x());
        double bboxHeight = fabs(p2.y() - p1.y());

        if (bboxWidth <= mSize.width() && bboxHeight <= mSize.height()) {
            break;
        }
    }

    mCenterCoord = startGeoRect.center();
    emit centerCoordChanged(mCenterCoord);
    emit zoomLevelChanged(mZoomLevel);
}

QPixmap RGOsmGraphicsView::renderMap()
{
    QPixmap outImage(mSize);
    QPainter painter(&outImage);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    //Don't draw the route and target rectangle on the generated map
    if (mRouteItem) mRouteItem->setVisible(false);
    if (mCutoutRectItem) mCutoutRectItem->setVisible(false);

    //Force selected resolution as output rectangle
    QRectF outputRect(0.0, 0.0, mSize.width(), mSize.height());
    qDebug() << "mCutoutRect=" << mCutoutRect << "; outputRect=" << outputRect;

    mScene->render(&painter, outputRect, mCutoutRect);
    painter.end();
    mOsmBackEnd.addAttribution(outImage);
    return outImage;
}

void RGOsmGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !tileLoadingInProgress())
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
    if (tileLoadingInProgress()) {
        //Tile loading is still in progress, reject new zoom requests
        event->ignore();
    }
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
    mCutoutRect = QRectF(topLeft, mSize);

    //We add some margin around the scene so the user can scroll the map. Later on (when rendering the map)
    //we will pick the mCutoutRect again, that represents the chosen resolution for the map.
    QRectF sceneRect = mCutoutRect.marginsAdded(QMarginsF(mOsmBackEnd.TILE_SIZE * marginTiles,
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

    centerOn(centerX, centerY);
}

void RGOsmGraphicsView::clearTiles()
{
    mScene->clear();
    mRouteItem = nullptr;
    mCutoutRectItem = nullptr;
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
        drawTargetRect();
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

void RGOsmGraphicsView::drawTargetRect()
{
    if (mCutoutRect.isEmpty()) return;

    mCutoutRectItem = mScene->addRect(mCutoutRect, QPen(Qt::black, 2));
}
