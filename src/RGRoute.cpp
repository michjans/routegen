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

//#define USE_SPLINE

#include "RGRoute.h"
#ifdef USE_SPLINE
using namespace std;
#include "spline.h"
#endif
#include <math.h>
#include <QDebug>

double RGLineSegment::getLength()
{
  double dx = (double) mX2 - mX1;
  double dy = (double) mY2 - mY1;
  return fabs(sqrt((dx * dx) + (dy * dy)));
}

QPoint RGLineSegment::getPoint(double distance)
{
  QPoint calcPoint;
  double alpha = atan2(double(mY2 - mY1), double(mX2 - mX1));
  double dy = sin(alpha) * distance;
  double dx = cos(alpha) * distance;
  calcPoint.setX(mX1 + dx);
  calcPoint.setY(mY1 + dy);

  return calcPoint;
}

//#define CIRCLE_ALGO

double RGCircleSegment::getLength()
{
  //TODO: Used in route interpolation, since getPoint is not implemented for
  //      circle segments, yet, just return the length of a straight line,
  //      later undef line below.
#ifdef CIRCLE_ALGO
  return fabs(mR * mPhi);
#else
  double dx = (double) mX2 - mX1;
  double dy = (double) mY2 - mY1;
  return fabs(sqrt((dx * dx) + (dy * dy)));
#endif
}

QPoint RGCircleSegment::getPoint(double distance)
{
  QPoint calcPoint;

#ifdef CIRCLE_ALGO
  //double circum = 2 * M_PI * mR;
  double alpha = -distance / mR;
  double dxi = mX1 - mXm;
  double dyi = mY1 - mYm;
  double beta = atan2(dyi, dxi);
  double gamma = beta + alpha;
  
  calcPoint.setX((int) cos(gamma) * mR);
  calcPoint.setY((int) sin(gamma) * mR);
#else
  //TODO: Now equal to RGLineSegment::getPoint, but should be rewritten
  //      to calculate coordinate on circle segment
  double alpha = atan2(double(mY2 - mY1), double(mX2 - mX1));
  double dy = sin(alpha) * distance;
  double dx = cos(alpha) * distance;
  calcPoint.setX(mX1 + dx);
  calcPoint.setY(mY1 + dy);

#endif //CIRCLE_ALGO

  return calcPoint;
}

RGRoute::RGRoute(const QList<QPoint> &path, int R)
: mR(R),
  mRawRoute(path)
{
  if (path.count() < 3) return;
  
  int i;
  int x1 = path[0].x();
  int y1 = path[0].y();
  int x2 = path[1].x();
  int y2 = path[1].y();
  int x3 = path[2].x();
  int y3 = path[2].y();
  for (i = 3; i < path.count(); i++)
  {
    addRouteSegments(x1, y1, x2, y2, x3, y3);
    //Note x1, y2 will be overwritten by addRouteSegments
    x2 = x3;
    y2 = y3;
    x3 = path[i].x();
    y3 = path[i].y();
  }
  //One more and finish with a last line segment
  addRouteSegments(x1, y1, x2, y2, x3, y3);
  mRoute.push_back(new RGLineSegment(x1, y1, x3, y3));
}

RGRoute::RGRoute(const QList<QPoint> &path)
: mR(0.0),
  mRawRoute(path)
{
  if (path.count() < 2) return;

  int x1 = path[0].x();
  int y1 = path[0].y();
  int x2 = path[1].x();
  int y2 = path[1].y();

  for (int i = 1; i < path.count(); i++)
  {
    mRoute.push_back(new RGLineSegment(x1, y1, x2, y2));
    x1 = x2;
    y1 = y2;
    x2 = path[i].x();
    y2 = path[i].y();
  }
  mRoute.push_back(new RGLineSegment(x1, y1, x2, y2));
}

RGRoute::~RGRoute()
{
  std::vector<RGRouteSegment *>::iterator i;
  for (i = mRoute.begin(); i != mRoute.end(); ++i)
  {
    delete *i;
  }

  mRoute.clear();
}


QList<QPoint>
RGRoute::getInterpolatedRoute(int npoints) const
{
  QList<QPoint> out;

//Test with spline lib
#ifdef USE_SPLINE

  //Interpolate x over distance and y over distance and combine interpolated
  double *xdata = new double[mRawRoute.size()];
  double *ydata = new double[mRawRoute.size()];
  double *tdata = new double[mRawRoute.size()];
  for (int i = 0; i < mRawRoute.size(); i++)
  {
    xdata[i] = (double) mRawRoute[i].x();
    ydata[i] = (double) mRawRoute[i].y();
    if (i == 0)
    {
      tdata[i] = 0.0;
    }
    else
    {
      //Add distance
      double dx = xdata[i] - xdata[i - 1];
      double dy = ydata[i] - ydata[i - 1];
      tdata[i] = tdata[i - 1] + sqrt((dx * dx) + (dy * dy));
    }
  }

  //Total distance interpolated over npoints (total distance is in last ddata point)
  double beta1 = 10.0;
  double beta2 = 10.0;
  double ddistance = tdata[mRawRoute.size() - 1] / npoints;
  double tval = 0.0;
  for (int i = 0; i < npoints; i++)
  {
    //int x = (int) spline_b_val (mRawRoute.size(), tdata, xdata, tval);
    //int y = (int) spline_b_val (mRawRoute.size(), tdata, ydata, tval);
    int x = (int) spline_beta_val (beta1, beta2, mRawRoute.size(), tdata, xdata, tval);
    int y = (int) spline_beta_val (beta1, beta2, mRawRoute.size(), tdata, ydata, tval);
    out.append(QPoint(x, y));
    tval += ddistance;
  }

  delete [] xdata;
  delete [] ydata;
  delete [] tdata;
#else

  if (mRoute.size() == 0) return out;

  double totalLength = 0.0;
  std::vector<RGRouteSegment *>::const_iterator i;
  for (i = mRoute.begin(); i != mRoute.end(); ++i)
  {
    totalLength += (*i)->getLength();
  }
  //Distance between each interpolated point
  double dDist = totalLength / npoints;
  double idist = dDist; //Start with dDist
  QPoint curPoint(mRoute[0]->getX1(), mRoute[0]->getY1());
  out.push_back(curPoint);
  for (unsigned int j = 0; j < mRoute.size(); ++j)
  {
    while (idist < mRoute[j]->getLength())
    {
      curPoint = mRoute[j]->getPoint(idist);
      out.push_back(curPoint);
      idist += dDist;
    }
    //Now calculate the remaining distance on next segment
    idist = idist - mRoute[j]->getLength();
  }
#endif


  return out;
}



//Adds new line/circle segment and overwites x1, y1 with the new values
//for the next iteration.
void
RGRoute::addRouteSegments(int &x1, int &y1, int x2, int y2, int x3, int y3)
{
  double dx1, dy1, dx2, dy2;
  double d1, d2, d3, d4;
  double alpha, phi1, phi2, phi3, phi4;
  double l, xm, ym;
  double xi, yi, xj, yj; //New intermediate points
  //Start with assumption of angle to the right
  bool rightAngle = true;
 
  while (true) {
    dx1 = (double) x2 - x1;
    dy1 = (double) y2 - y1;
    d1 = sqrt((dx1 * dx1) + (dy1 * dy1));
    dx2 = (double) x3 - x2;
    dy2 = (double) y3 - y2;
    d2 = sqrt((dx2 * dx2) + (dy2 * dy2));
    phi1 = atan2(dy1, dx1);
    phi2 = atan2(dy2, dx2);
    alpha = (phi1 - phi2) / 2.0;
    d4 = mR * tan(alpha);

    if (d4 > 0.0 || !rightAngle) {
      //Angle to the right, ok
      break;
    } else {
      //Angle to the left, swap points so we make it a right angle
      rightAngle = false;
      int tmpx3 = x3;
      int tmpy3 = y3;
      x3 = x1;
      y3 = y1;
      x1 = tmpx3;
      y1 = tmpy3;
    }
  }
  d3 = d1 - d4;
  phi3 = atan2(mR, d3);
  phi4 = phi1 - phi3;
  l = sqrt((d3 * d3) + (mR * mR));
  xm = l * cos(phi4) + x1;
  ym = l * sin(phi4) + y1;
  xi = x1 + cos(phi1) * d3;
  yi = y1 + sin(phi1) * d3;
  xj = x2 + cos(phi2) * d4;
  yj = y2 + sin(phi2) * d4;

  if (!rightAngle) {
    //Swap back points
    int tmpx3 = x3;
    int tmpy3 = y3;
    x3 = x1;
    y3 = y1;
    x1 = tmpx3;
    y1 = tmpy3;

    double tmpxj = xj;
    double tmpyj = yj;
    xj = xi;
    yj = yi;
    xi = tmpxj;
    yi = tmpyj;
  }
  
  //Add line/circle segment
  mRoute.push_back(new RGLineSegment(x1, y1, xi, yi));
  mRoute.push_back(new RGCircleSegment(xi, yi, xj, yj, xm, ym, 2*alpha, mR));

  //The next segment will now start at xj, yj, instead of x2, y2, so overwrite.
  x1 = xj;
  y1 = yj;
}

void
RGRoute::dump()
{
  std::vector<RGRouteSegment *>::iterator i;
  qDebug() << "RGRoute::dump()=======================================";
  for (i = mRoute.begin(); i != mRoute.end(); ++i)
  {
    qDebug() << (*i)->mX1 << ", " << (*i)->mY1 << " --> " << (*i)->mX2 << ", " << (*i)->mY2;
  }
}
