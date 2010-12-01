/*
    Copyright (C) 2009  Michiel Jansen

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

#ifndef RG_ROUTE_H
#define RG_ROUTE_H


#include <vector>
#include <QList>
#include <QPoint>

class RGRouteSegment
{
friend class RGRoute; //TODO: Remove (only used by dump)

public:
  virtual double getLength() = 0;

  //Returns point with distance in pixels from start point
  virtual QPoint getPoint(double distance) = 0; 

  int getX1() const { return mX1; } 
  int getY1() const { return mY1; }
  int getX2() const { return mX2; }
  int getY2() const { return mY2; }


protected:
  RGRouteSegment(int x1, int y1, int x2, int y2)
    : mX1(x1), mY1(y1), mX2(x2), mY2(y2) 
  {}
  
  int mX1, mX2, mY1, mY2;
};

class RGLineSegment : public RGRouteSegment
{
friend class RGRoute;

public:
  RGLineSegment(int x1, int y1, int x2, int y2)
    : RGRouteSegment(x1, y1, x2, y2)
  {}

  double getLength();
  QPoint getPoint(double distance); 

};

class RGCircleSegment : public RGRouteSegment
{
friend class RGRoute;

public:
  RGCircleSegment(int x1, int y1, int x2, int y2, int xm, int ym, double phi, int r)
    : RGRouteSegment(x1, y1, x2, y2),
      mXm(xm), mYm(ym), mPhi(phi), mR(r)
  {}

  double getLength();
  QPoint getPoint(double distance); 

  int getXm() const { return mXm; }
  int getYm() const { return mYm; }
  int getR()  const { return mR; }

private:
  int mXm, mYm, mR;
  double mPhi; //Angle of circle segment

};

class RGRoute
{
public:
  //Creates a route with line and circles segments, calculated from passed path
  RGRoute(const QList<QPoint> &path, int R);

  //Creates a route with only line segments
  //Introduced this constructor to not add circle segments anymore, because
  //the interpolation algorithm doesn't work yet (see CIRCLE_ALGO define in RGRoute.cpp)
  //Leaving circle segments in results in 'shortcut' lines
  RGRoute(const QList<QPoint> &path);

  ~RGRoute();

  //Returns the route interpolated in npoints
  QList<QPoint> getInterpolatedRoute(int npoints) const;
  const QList<QPoint> &getRawRoute() const {return mRawRoute;}

  const std::vector<RGRouteSegment *>& getSegments() const { return mRoute; }

  void dump();

private:
  void addRouteSegments(int &x1, int &y1, int x2, int y2, int x3, int y3);

private:
  std::vector<RGRouteSegment *> mRoute;
  QList<QPoint>                 mRawRoute;
  int                           mR; //Radius to use for circle segments
};


#endif //RG_ROUTE_H