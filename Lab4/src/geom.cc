/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

/*
 * Includes published pubic domain information and code from the following sources:
 *
 * https://math.stackexchange.com/questions/275529/check-if-line-intersects-with-circles-perimeter
 * https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
 * https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
 */

#include <iostream>

#include <string>
#include <math.h>

#include "geom.h"

double Geom::distance(const Point2D &a, const Point2D &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double Geom::distance(const Point3D &a, const Point3D &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

bool Geom::intersects(const Line &line, Circle &circle) {
    return (abs((circle.center().y - line.a.y) * (line.b.x - line.a.x) -
        (circle.center().x -  line.a.x) * (line.b.y - line.a.y)) /
            distance(line.a, line.b)) < circle.radius();
}

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
static bool onSegment(Point2D p, Point2D q, Point2D r)
{
    return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
}

// Finds the orientation of ordered triplet (p, q, r).
// Returns:
//      0 --> p, q and r are colinear
//      1 --> Clockwise
//      2 --> Counterclockwise
static int orientation(Point2D p, Point2D q, Point2D r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool Geom::intersects(const Line &a, const Line &b)
{
    int o1 = orientation(a.a, a.b, b.a);
    int o2 = orientation(a.a, a.b, b.b);
    int o3 = orientation(b.a, b.b, a.a);
    int o4 = orientation(b.a, b.b, a.b);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // a.a, a.b and b.a are colinear and b.a lies on segment a.a a.b
    if (o1 == 0 && onSegment(a.a, b.a, a.b)) return true;

    // a.a, a.b and b.b are colinear and b.b lies on segment a.a a.b
    if (o2 == 0 && onSegment(a.a, b.b, a.b)) return true;

    // b.a, b.b and a.a are colinear and a.a lies on segment b.a b.b
    if (o3 == 0 && onSegment(b.a, a.a, b.b)) return true;

     // b.a, b.b and a.b are colinear and a.b lies on segment b.a b.b
    if (o4 == 0 && onSegment(b.a, a.b, b.b)) return true;

    return false;
}

Point2D Geom::center(RegularConvexPolygon &polygon)
{
    double cx = 0.0;
    double cy = 0.0;
    double signedArea = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    double a = 0.0;  // Partial signed area

    for (int i = 0; i < polygon.vertexCount(); i++)
    {
        x0 = polygon.vertices()[i].x;
        y0 = polygon.vertices()[i].y;
        x1 = polygon.vertices()[(i+1) % polygon.vertexCount()].x;
        y1 = polygon.vertices()[(i+1) % polygon.vertexCount()].y;
        a = x0*y1 - x1*y0;
        signedArea += a;
        cx += (x0 + x1)*a;
        cy += (y0 + y1)*a;
    }

    signedArea *= 0.5;
    cx /= (6.0*signedArea);
    cy /= (6.0*signedArea);

    return Point2D(cx,cy);
}
