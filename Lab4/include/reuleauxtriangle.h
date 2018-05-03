/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _REULEAUXTRIANGLE_H_
#define _REULEAUXTRIANGLE_H_

#include <vector>

#include "containable.h"
#include "point.h"
#include "circle.h"
#include "geom.h"
#include "line.h"

// C++ requires forward declaration of any cyclically dependent classes
class Circle;

class ReuleauxTriangle : public Containable2D {
    private:
        Point2D vertices_[3];
    public:
        ReuleauxTriangle(Point2D vertices[3]);

        Point2D* vertices();
        float sideLength(){ return Geom::distance(vertices_[0],vertices_[1]);}
        bool containsPoint(const Point2D&);

        bool containedWithin(Circle &circle);
        bool containedWithin(RegularConvexPolygon &polygon);
        bool containedWithin(ReuleauxTriangle &rt);
};

#endif
