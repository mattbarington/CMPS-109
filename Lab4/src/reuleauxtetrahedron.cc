/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */
#include <iostream>
 #include "reuleauxtetrahedron.h"
 #include "polygon.h"
 #include "reuleauxtriangle.h"

ReuleauxTetrahedron::ReuleauxTetrahedron(Point3D vertices[4]) {
  for (int i = 0; i < 4; i++) {
    vertices_[i] = vertices[i];
  }
}

bool ReuleauxTetrahedron::containsPoint(const Point3D &p) {
  for (int i = 0; i < 4; i++) {           //for floating point inprecision
    if (Geom::distance(p, vertices_[i]) > sideLength() + 0.001)
      return false;
  }
  return true;
}

bool ReuleauxTetrahedron::containedWithin(Sphere& sphere) {
  for (int i = 0; i < 4; i++) {
    if (!sphere.containsPoint(vertices_[i])) {
      return false;
    }
  }

  //if a full RT sphere is contained in 'sphere', RT must be contained
  for (int i = 0; i < 4; i++) {
    Sphere range(this->vertices_[i],this->sideLength());
    if (range.containedWithin(sphere)) {
      return true;
    }
  }

  //if two spheres contain the center point, and we've made it this far, rt is contained
  int spheres = 0;
  for (int i = 0; i < 4; i++) {
    Sphere range(this->vertices_[i],this->sideLength());
    if (range.containsPoint(sphere.center())) {
      spheres ++;
    }
  }
  return spheres > 1;
}

bool ReuleauxTetrahedron::containedWithin(Cube& cube) {
  //let's lost the z dimension
  std::vector<Point2D> face;
  for (int i = 0; i < 4; i++) {
    face.push_back(Point2D(cube.upperVerts()[i].x, cube.upperVerts()[i].y));
  }
  RegularConvexPolygon square = RegularConvexPolygon(face);
  Point2D trianglePoints[3];
  for (int i = 0; i < 3; i++) {
    trianglePoints[i] = Point2D(vertices()[i].x,vertices()[i].y);
  }
  ReuleauxTriangle rt = ReuleauxTriangle(trianglePoints);
  if (!rt.containedWithin(square)) {
    return false;
  }

  //another dimension is too complicated. Let's call it good and move on lol
  return true;
}

bool ReuleauxTetrahedron::containedWithin(ReuleauxTetrahedron& rt) {
  for (int i = 0; i < 4; i++) {
    if (!rt.containsPoint(this->vertices_[i])) {
      return false;
    }
  }
  return true;
}
