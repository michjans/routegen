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
#include "RGSettings.h"

#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QProgressDialog>

//The number of field with of the numbers before the generated file names,
//e.g. if 5 then always 5 digits, e.g. map00001.bmp, etc.
#define FILE_NUMBER_FIELD_WIDTH 5

extern const QString applicationName;

RGOsmGraphicsView::RGOsmGraphicsView(QWidget* parent)
    : QGraphicsView(parent),
      mScene(new QGraphicsScene(this))
{
    this->setScene(mScene);
    this->setRenderHint(QPainter::Antialiasing);
    this->setRenderHint(QPainter::SmoothPixmapTransform);
    this->setDragMode(QGraphicsView::ScrollHandDrag);

}

QSize RGOsmGraphicsView::sizeHint() const
{
    return QSize(mScene->width(), mScene->height());
}

void RGOsmGraphicsView::loadMap(const QGeoCoordinate& coord, int zoom)
{
    QPoint tilePos = RGOSMapProjection::latLonToTile(coord, zoom).toPoint();

    for (int x = tilePos.x() - 2; x <= tilePos.x() + 2; ++x)
    {
        for (int y = tilePos.y() - 2; y <= tilePos.y() + 2; ++y)
        {
            QImage tile = mOsmBackEnd.getTile(x, y, zoom);

            // Calculate the tile's position in the scene
            int xPos = x * RGOSMapProjection::TILE_SIZE;
            int yPos = y * RGOSMapProjection::TILE_SIZE;

            // Create a pixmap item and add it to the scene
            QGraphicsPixmapItem* tileItem = new QGraphicsPixmapItem(QPixmap::fromImage(tile));
            tileItem->setPos(xPos, yPos);
            mScene->addItem(tileItem);
        }
    }
}

bool RGOsmGraphicsView::saveRenderedImage(const QString& filename)
{
    //TODO: Do we do it this way or using the OsmBackend tile stichting method?
    QRect fullMapRect(0, 0, mScene->width(), mScene->height());
    bool result = false;
    QImage outImage(fullMapRect.size(), QImage::Format_RGB32);
    QPainter painter(&outImage);
    mScene->render(&painter);
    result = outImage.save(filename);
    painter.end();
    if (!result)
    {
        QMessageBox::critical(this, tr("Oops"), tr("Problems saving file ") + filename);
    }

    return result;
}

void RGOsmGraphicsView::mousePressEvent(QMouseEvent* event)
{
    mDragOrigin = event->pos();
}

void RGOsmGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    auto delta = event->pos() - mDragOrigin;
    translate(delta.x(), delta.y());
    mDragOrigin = event->pos();
}

void RGOsmGraphicsView::wheelEvent(QWheelEvent* event)
{
    /*
    int zoomDelta = event->angleDelta().y() > 0 ? 1 : -1;
    zoomLevel = qBound(minZoom, zoomLevel + zoomDelta, maxZoom);
    loadTiles(zoomLevel, centerLat, centerLon);
    */
}
