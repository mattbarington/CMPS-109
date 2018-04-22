/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "point.h"
#include "line.h"
#include "circle.h"
#include "polygon.h"

class Geom {
    public:
        static double  distance(const Point2D &a, const Point2D &b);
        static bool    intersects(const Line &line, Circle &circle);
        static bool    intersects(const Line &a, const Line &b);
        static Point2D center(RegularConvexPolygon &polygon);
};

#endif
