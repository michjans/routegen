/*
    Copyright (C) 2009-2011  Michiel Jansen
    Copyright (C) 2010-2011  Fabien Valthier

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

#include "RGVehicleOriginPt.h"

#include "RGVehicle.h"

RGVehicleOriginPt::RGVehicleOriginPt(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    setPos(0, 0);
    setCursor(Qt::CrossCursor);
}

QRectF RGVehicleOriginPt::boundingRect() const
{
    return this->parentItem()->boundingRect();
}

void RGVehicleOriginPt::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

void RGVehicleOriginPt::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    RGVehicle* vehicle = qgraphicsitem_cast<RGVehicle*>(this->parentItem());
    if (vehicle != 0)
    {
        qDebug() << "new Origin" << event->pos() - this->boundingRect().topLeft();
        vehicle->setOrigin(event->pos() - this->boundingRect().topLeft());
    }
}

void RGVehicleOriginPt::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //setOrigin(event->pos());
}
