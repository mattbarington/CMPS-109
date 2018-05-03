/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "containable.h"
#include "point.h"
#include "cube.h"
#include "reuleauxtetrahedron.h"

class Sphere : public Containable3D {
    private:
        Point3D center_;
        double radius_;

    public:
        //Sphere(Point3D &center, double radius);
        Sphere(Point3D center, double radius);

        Point3D center();
        void setCenter(const Point3D &center);

        double radius();
        void setRadius(double radius);
        bool containsPoint(const Point3D&);

        bool containedWithin(Sphere &sphere);
        bool containedWithin(Cube &cube);
        bool containedWithin(ReuleauxTetrahedron &rt);
};

#endif
