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

#include "RGPath.h"

#include <cmath>
#include <deque>

namespace
{
const int gVehicleLookBack = 1;
const size_t gAvgAngleWindowSize = 5;
}

RGPath::RGPath(QGraphicsItem* parent)
    : QGraphicsObject(parent),
      mRawPath(QList<QPoint>()),
      mTime(0),
      mPath(QPainterPath()),
      mPaintPath(QPainterPath()),
      mTotalTime(0),
      mPlayMode(0),
      mFPS(25),
      mDSmooth(10),
      mSmoothPath(false),
      mPen(QPen()),
      mCurrentFrame(0),
      mEndPos(QPoint())
{
}

QRectF RGPath::boundingRect() const
{
    qreal penW = mPen.width() / 2;
    return mPath.boundingRect().adjusted(-penW, -penW, penW, penW);
}

void RGPath::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(mPen);
    painter->drawPath(mPaintPath);
}

QPainterPath RGPath::getPathAtStep(int step) //return Path at step (begin at 0)
{
    if (step >= mPath.elementCount())
        return mPath;

    QPainterPath tmpPath;
    tmpPath.moveTo(mPath.elementAt(0).x, mPath.elementAt(0).y);
    for (int i = 1; i <= step; ++i)
    {
        tmpPath.lineTo(mPath.elementAt(i).x, mPath.elementAt(i).y);
    }
    return tmpPath;
}

QPainterPath RGPath::getPathAtTime(int time) //return Path at time in ms
{

    QPointF newpointf;
    qreal percent = (double)time / ((double)mTotalTime * 1000); //mTotalTime should never be null
    if (percent >= 1)
        return mPath;
    if (mPath.elementCount() == 0)
        return QPainterPath();
    newpointf = mPath.pointAtPercent(percent);
    qreal newlength = percent * mPath.length();
    //Now locate on which segment the length is obtained:
    QPainterPath goodPath, testPath;
    testPath.moveTo(mPath.elementAt(0).x, mPath.elementAt(0).y);
    goodPath.moveTo(mPath.elementAt(0).x, mPath.elementAt(0).y);
    for (int i = 1; i < mPath.elementCount(); ++i)
    {
        testPath.lineTo(mPath.elementAt(i).x, mPath.elementAt(i).y);
        if (testPath.length() >= newlength)
        {
            //the i element contains the point at time
            goodPath.lineTo(newpointf);
            break;
        }
        //if no break, the whole segment is added to the goodPath
        goodPath.lineTo(mPath.elementAt(i).x, mPath.elementAt(i).y);
    }
    return goodPath;
}

int RGPath::countFrames()
{
    //stepbystep
    if (mPlayMode == 0)
        return mPath.elementCount();
    //Interpolation TotalTime set
    if (mPlayMode == 1 && mPath.elementCount() > 1)
        return mFPS * mTotalTime + 1;
    return 0;
}

void RGPath::setCurrentFrame(int frame)
{
    //check that frame is in the total range
    if (frame >= 0 && frame < this->countFrames())
        mCurrentFrame = frame;
    else if (this->countFrames() > 0)
        mCurrentFrame = this->countFrames() - 1;
    else
        mCurrentFrame = 0;

    //create new mPaintPath
    mPaintPath = QPainterPath();
    int time = mCurrentFrame * (1.0 / (double)mFPS) * 1000;
    if (mPlayMode == 0)
        mPaintPath = getPathAtStep(mCurrentFrame);
    if (mPlayMode == 1)
        mPaintPath = getPathAtTime(time);
    if (mPaintPath.elementCount() >= 1)
        mEndPos = QPointF(mPaintPath.elementAt(mPaintPath.elementCount() - 1).x, mPaintPath.elementAt(mPaintPath.elementCount() - 1).y);

    update();
}

int RGPath::getCurrentFrame()
{
    return mCurrentFrame;
}

int RGPath::getCurrentTime()
{
    return mCurrentFrame * (1.0 / (double)mFPS) * 1000;
}

QPointF RGPath::getEndPos()
{
    return mEndPos;
}

float RGPath::getAngle()
{
    //To smooth out the vehicle rotations, we return the average angle over a period of gAvgAngleWindowSize frames
    return (360.0f - mAvgAngles.at(mCurrentFrame));
}

void RGPath::newPointList(const QList<QPoint>& pointList)
{
    if (pointList.size() < 0)
        return;
    mRawPath = pointList;
    createPath();
    this->setCurrentFrame(this->countFrames());
}

void RGPath::setDrawTime(int time)
{
    mTime = time;
}

void RGPath::setSmoothPath(bool smooth)
{
    mSmoothPath = smooth;
    createPath();
}

void RGPath::setSmoothCoef(int dSmooth)
{
    mDSmooth = dSmooth;
    createPath();
}

void RGPath::setTotalTime(int time) //set mTotaltime in ms !!it must never be null
{
    if (time != 0)
        mTotalTime = time;
}

void RGPath::setPlayMode(int mode)
{
    if (mode >= 0 && mode < 3)
        mPlayMode = mode;
    else
        mPlayMode = 0;
    createPath();
}

void RGPath::setFPS(int FPS)
{
    mFPS = FPS;
}

void RGPath::setPen(const QPen& pen)
{
    this->prepareGeometryChange();
    mPen = pen;
    update();
}

void RGPath::createPath()
{
    qDebug() << "RGPath::createPathh: mRawPath.size():" << mRawPath.size();
    prepareGeometryChange();
    //create path from data :
    mPath = QPainterPath();
    if (mRawPath.count() <= 0)
    {
        update();
        return;
    }
    if (mSmoothPath)
    {
        createSmoothPath();
    }
    else
    {
        mPath.moveTo(mRawPath.at(0));
        for (int i = 1; i < mRawPath.count(); ++i)
        {
            mPath.lineTo(mRawPath.at(i));
        }
    }

    createAverageRouteAngles();
}

void RGPath::createSmoothPath()
{
    mPath = QPainterPath();
    if (mRawPath.size() <= 1)
        return;
    QPainterPath newPath(mRawPath.at(0));
    QPoint A = mRawPath.at(0), B = mRawPath.at(1), p1, c1, p2, c2, sc2;
    int dist1 = 0;
    bool haveP2 = false;
    double dAB = sqrt(pow(double((B - A).x()), 2) + pow(double((B - A).y()), 2));
    c1 = B;
    if (dAB > mDSmooth)
    {
        p1 = getPointAtLength(B, A, mDSmooth);
    }
    else
    {
        p1 = getPointAtLength(A, B, 1); //line one point before connect(path)
    }
    newPath.lineTo(p1);

    for (int i = 1; i < mRawPath.count() - 1; ++i)
    {
        A = mRawPath.at(i);
        B = mRawPath.at(i + 1);
        dAB = sqrt(pow(double((B - A).x()), 2) + pow(double((B - A).y()), 2));
        if (haveP2)
        {
            dist1 += (int)dAB;
            if (dist1 >= mDSmooth)
            {
                sc2 = getPointAtLength(B, A, dist1 - mDSmooth);
                c2 = 2 * p2 - sc2;
                newPath.connectPath(pathLineCubic(p1, (c1 - p1) / 4 + p1, (c2 - p2) / 4 + p2, p2));
                p1 = p2;
                c1 = 2 * p1 - c2;
                if (dAB > mDSmooth)
                {
                    haveP2 = false;
                    p2 = getPointAtLength(A, B, mDSmooth);
                    c2 = A;
                    newPath.connectPath(pathLineCubic(p1, (c1 - p1) / 4 + p1, (c2 - p2) / 4 + p2, p2));
                    dist1 = dAB - mDSmooth;
                    p1 = p2;
                    c1 = 2 * p1 - c2;
                    if (dAB > 2 * mDSmooth)
                    {
                        p1 = getPointAtLength(B, A, mDSmooth);
                        newPath.lineTo(p1);
                        c1 = B;
                        dist1 = 0;
                    }
                }
                else
                {
                    p2 = c1;
                    dist1 = sqrt(pow(double((B - p2).x()), 2) + pow(double((B - p2).y()), 2));
                }
            }
        }
        else
        {
            if (dAB >= 2 * mDSmooth)
            {
                c1 = A;
                p2 = getPointAtLength(A, B, mDSmooth);
                newPath.connectPath(pathLineQuad(p1, c1, p2));
                //newPath.addRect(c1.x(),c1.y(),2,2);
                p1 = getPointAtLength(B, A, mDSmooth);
                newPath.lineTo(p1);
                c1 = B;
            }
            else
            {
                dist1 += (int)dAB;
                if (dist1 >= mDSmooth)
                {
                    p2 = getPointAtLength(B, A, dist1 - mDSmooth);
                    haveP2 = true;
                    dist1 = sqrt(pow(double((B - p2).x()), 2) + pow(double((B - p2).y()), 2));
                }
            }
        }
    }
    if (dAB >= 2 * mDSmooth)
        newPath.lineTo(B);
    else
        newPath.connectPath(pathLineCubic(p1, (c1 - p1) / 4 + p1, (A - B) / 4 + B, B));
    mPath = newPath;
}

void RGPath::createAverageRouteAngles()
{
    //Fill a sliding window of 10 angles from the route in a deque and calculate the average angle over that period
    mAvgAngles.clear();
    std::deque<double> slidingAngleWindow;
    int frameCount = countFrames();
    for (int frameIdx = 0; frameIdx < frameCount; ++frameIdx)
    {
        slidingAngleWindow.push_back(getAngleAtFrame(frameIdx) * M_PI / 180.0);
        if (slidingAngleWindow.size() > gAvgAngleWindowSize)
        {
            slidingAngleWindow.pop_front();
        }

        // calculate sums of sine and cosine of all angles
        double sum_sin = 0.0;
        double sum_cos = 0.0;
        for (double angle_rad : slidingAngleWindow)
        {
            sum_sin += std::sin(angle_rad);
            sum_cos += std::cos(angle_rad);
        }

        // calculate average angle in radians
        double avg_angle_rad = std::atan2(sum_sin, sum_cos);

        // convert average angle to degrees
        mAvgAngles.emplace_back(avg_angle_rad * 180.0 / M_PI);
    }
}

QPoint RGPath::getPointAtLength(QPoint start, QPoint end, int length)
{
    QPoint dv = end - start;
    double totlength = sqrt(pow(double(dv.x()), 2) + pow(double(dv.y()), 2));

    if (totlength == 0)
    {
        return start; // Prevent division by zero
    }

    return start + (length / totlength) * dv;
}

double RGPath::getAngleAtFrame(int frameIdx)
{
    qreal angle = 0;
    if (mPlayMode == 0)
    {
        //NOTE: Non-interpolation mode! (rarely used, could probably be removed completely)
        if (mPath.elementCount() <= 1)
            return 0;
        int step = frameIdx;
        //for the first frame :
        if (step == 0)
        {
            angle = QLineF(mPath.elementAt(0).x, mPath.elementAt(0).y, mPath.elementAt(1).x, mPath.elementAt(1).y).angle();
        }
        else if (step > gVehicleLookBack)
        {
            //Smooth movement of vehicle rotation
            angle =
                QLineF(mPath.elementAt(step - gVehicleLookBack).x, mPath.elementAt(step - gVehicleLookBack).y, mPath.elementAt(step).x, mPath.elementAt(step).y)
                    .angle();
        }
        else
        {
            angle = QLineF(mPath.elementAt(0).x, mPath.elementAt(0).y, mPath.elementAt(step).x, mPath.elementAt(step).y).angle();
        }
    }
    if (mPlayMode == 1)
    {
        //NOTE: Interpolation mode! (most commonly used)
        qreal percent = (double)(frameIdx * (1.0 / (double)mFPS)) / ((double)mTotalTime); //mTotalTime should never be null;
        if (percent > 1)
            percent = 1;
        angle = mPath.angleAtPercent(percent);
    }

    return angle;
}

QPainterPath RGPath::pathLineQuad(QPoint start, QPoint coef, QPoint end)
{
    QPainterPath curvePath(start), linearPath(start);
    curvePath.quadTo(coef, end);
    int nbPoints = (int)curvePath.length() / 4;
    for (int i = 0; i < nbPoints; i++)
    {
        linearPath.lineTo(curvePath.pointAtPercent(i * 1.0 / nbPoints));
    }
    linearPath.lineTo(end);
    return linearPath;
}

QPainterPath RGPath::pathLineCubic(QPoint start, QPoint coef1, QPoint coef2, QPoint end)
{
    QPainterPath curvePath(start), linearPath(start);
    curvePath.cubicTo(coef1, coef2, end);
    int nbPoints = (int)curvePath.length() / 4;
    for (int i = 0; i < nbPoints; i++)
    {
        linearPath.lineTo(curvePath.pointAtPercent(i * 1.0 / nbPoints));
    }
    linearPath.lineTo(end);
    return linearPath;
}
