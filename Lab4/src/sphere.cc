/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include "sphere.h"
#include "reuleauxtetrahedron.h"
#include "circle.h"
#include "polygon.h"


// Sphere::Sphere(Point3D &center, double radius) {
//   center_.x = center.x;
//   center_.y = center.y;
//   center_.z = center.z;
//   radius_  = radius;
// }


Sphere::Sphere(Point3D center, double radius) {
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

double Sphere::radius() {
  return radius_;
}

bool Sphere::containsPoint(const Point3D &p) {
  return !(Geom::distance(center_, p) > radius_);
}

bool Sphere::containedWithin(Sphere &sphere) {
  return Geom::distance(sphere.center_,center_) <= (sphere.radius_ - radius_);
}
bool Sphere::containedWithin(Cube &cube) {
  std::vector<Point2D> face;
  //be rid of the z dimension
  for (int i = 0; i < 4; i++) {
    face.push_back(Point2D(cube.upperVerts()[i].x, cube.upperVerts()[i].y));
  }
  Circle lostDim = Circle(Point2D(center().x, center().y), radius());
  RegularConvexPolygon square = RegularConvexPolygon(face);
  if (!lostDim.containedWithin(square)) {
    return false;
  }

  face.clear();
  //be rid of the x dimension
  for (int i = 0; i < 2; i++) {
    face.push_back(Point2D(cube.upperVerts()[i].y,cube.upperVerts()[i].z));
    face.push_back(Point2D(cube.lowerVerts()[i].y,cube.upperVerts()[i].z));
  }
  lostDim = Circle(Point2D(center().y, center().z), radius());
  square = RegularConvexPolygon(face);
  return lostDim.containedWithin(square);
}
bool Sphere::containedWithin(ReuleauxTetrahedron &rt) {
  float side = rt.sideLength();
  for (int i = 0; i < 4; i++) {
    Sphere range(rt.vertices()[i],side);
    if (!this->containedWithin(range)) {
      return false;
    }
  }
  return true;
}
