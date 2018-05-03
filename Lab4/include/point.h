/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _POINT_H_
#define _POINT_H_

#include <string>
using std::to_string;

class Point2D {
    public:
        double x;
        double y;

        std::string toString() {
          return "Point2D("+to_string(x)+","+to_string(y)+")";
        }

        // do not change these constructors
        Point2D() { x = 0.0, y = 0.0; }
        Point2D(double dx, double dy) { x = dx; y = dy; }
        Point2D(const Point2D &p) { x = p.x; y = p.y; }
};

class Point3D : public Point2D {
    public:
        double z;

        std::string toString() {
          return "Point2D("+to_string(x)+","+to_string(y)+","+to_string(z)+")";
        }

        // do not change these constructors
        Point3D() : Point2D() { z = 0.0; }
        Point3D(double dx, double dy, double dz) : Point2D(dx, dy) { z = dz; }
        Point3D(const Point3D &p) : Point2D(p.x, p.y) { z = p.z; }
};

#endif
