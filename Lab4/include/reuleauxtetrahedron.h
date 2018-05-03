/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _REULEAUXTETRAHEDRON_H_
#define _REULEAUXTETRAHEDRON_H_

#include "containable.h"
#include "point.h"
#include "sphere.h"
#include "cube.h"
#include "geom.h"

class ReuleauxTetrahedron : public Containable3D {
  private:
    Point3D vertices_[4];

  public:
    ReuleauxTetrahedron(Point3D vertices[4]);

    Point3D* vertices() {return vertices_;};
    float sideLength() {return Geom::distance(vertices_[0],vertices_[1]);}
    bool containsPoint(const Point3D &p);

    bool containedWithin(Sphere &sphere);
    bool containedWithin(Cube &cube);
    bool containedWithin(ReuleauxTetrahedron &rt);
};

#endif
