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

#ifndef RGEDITPATH_H
#define RGEDITPATH_H

#include "RGEditPathPoint.h"
#include <QGraphicsObject>
#include <QtGui>

class RGEditPath : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RGEditPath(QGraphicsItem* parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    const QList<RGEditPathPoint*>& path() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

signals:
    void newPointList(QList<QPoint>, bool);

public slots:
    void clear(bool canUndo = true);
    void setNewPoints(const QList<QPoint>& points);
    void editPathPointMoved(bool canUndo = true);
    void editPathPointAdd(RGEditPathPoint*);
    void on_sceneRectChanged();

private:
    void updatePointList(bool canUndo = true);
    void addPoint(QPoint);
    void deleteSelectedPoints();

private:
    QRectF mBoundingRect;
    QList<RGEditPathPoint*> mEditPathPointList;
    bool mMousePressed;
    bool mFreeDraw;
    bool mSelect;
};

#endif // RGEDITPATH_H
