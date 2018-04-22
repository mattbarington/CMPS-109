/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <vector>

#include "containable.h"
#include "point.h"
#include "circle.h"
#include "line.h"

// C++ requires forward declaration of any cyclically dependent classes
class Circle;

class RegularConvexPolygon : public Containable2D {
    private:
        std::vector<Point2D> vertices_;

    public:
        // do not change this constructor
        RegularConvexPolygon(std::vector<Point2D> vertices);

        std::vector<Point2D> vertices();
        std::vector<Line> edges();
        int vertexCount();
        float maxXCoordinate();

        bool containedWithin(Circle &circle);
        bool containedWithin(RegularConvexPolygon &polygon);
        bool containedWithin(ReuleauxTriangle &rt);
};

#endif
