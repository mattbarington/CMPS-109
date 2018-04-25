/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include "sphere.h"


Sphere::Sphere(const Point3D &center, double radius) {
  center_.x = center.x;
  center_.y = center.y;
  center_.z = center.z;
  radius_  = radius;
}

Point3D Sphere::center() {
  return center_;
}

void Sphere::setCenter(const Point3D &c) {
  center_.x = c.x;
  center_.y = c.y;
  center_.z = c.z;
}

bool Sphere::containedWithin(Sphere &sphere) {
  return Geom::distance(sphere.center_,center_) <= (sphere.radius_ - radius_);
}
bool Sphere::containedWithin(Cube &cube) {
  throw "Not implemented";
}
bool Sphere::containedWithin(ReuleauxTetrahedron &rt) {
  throw "Not Implemented";
}
