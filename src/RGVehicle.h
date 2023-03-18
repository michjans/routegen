/*
    Copyright (C) 2008-2011  Michiel Jansen
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

#ifndef RGVEHICLE_H
#define RGVEHICLE_H

#include <QGraphicsItem>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <vector>

class RGVehicle : public QGraphicsItem
{
public:
    RGVehicle(const QString& filename = "None", int size = 0, bool mirror = false, int startAngle = 0, bool acceptRotation = true,
              QPointF originPoint = QPointF(-1, -1), int frameDelay = 80);
    ~RGVehicle();
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    enum
    {
        Type = UserType + 1
    };
    int type() const
    {
        return Type;
    }

    QPointF getOrigin();
    int getSize();
    int getRawSize();
    bool getMirror();
    int getStartAngle();
    int getDelay();
    QPixmap getPixmapAtSize(int);
    QString getName() const;
    QString getFileName() const;
    bool isCustom() const
    {
        return mIsCustom;
    }

    void setOrigin(QPointF point);
    void setSize(int size);
    void setMirror(bool mirror);
    void setStartAngle(int selfAngle);
    void setRotation(qreal angle);
    void setTime(int time);
    void setIsCustom(bool isCustom)
    {
        mIsCustom = isCustom;
    }

    /**
     * Set if the vehicle should accept rotations
     * @param doRotation if true vehicle will accept calls to setRotation (default)
     */
    void acceptRotation(bool doRotation)
    {
        mAcceptRotation = doRotation;
    }

    bool acceptsRotation() const
    {
        return mAcceptRotation;
    }

private:
    QString mFileName;
    bool mMirror;
    int mStartAngle;
    int mSize;
    int mRawSize;
    int mFrameDelay;
    int mCurrentPm;
    std::vector<QPixmap> mRawPm;
    QPointF mOriginPoint;
    bool mRotMirror;
    bool mAcceptRotation;
    bool mIsCustom;
};

#endif // RGVEHICLE_H
