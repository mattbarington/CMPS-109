/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include "geom.h"
#include "polygon.h"
#include "reuleauxtriangle.h"

ReuleauxTriangle::ReuleauxTriangle(Point2D* vertices) {
  vertices_ = vertices;
}

Point2D* ReuleauxTriangle::vertices() {
  return vertices_;
}

/*
 * All vertices of the reuleaux triangle must be contained within the circle.
 * If the curvature of the triangle is smaller than the curvature of the
 * containing circle, then
 */
bool ReuleauxTriangle::containedWithin(Circle &circle) {
  //bool withinSideLength = true;
  for (int i = 0; i < 3; i++) {
    if (Geom::distance(circle.center(), vertices_[i]) > circle.radius()) {
       printf("Distance from p: (%f,%f) <-> c: (%f,%f) > r:%f\n",vertices_[i].x,vertices_[i].y,
                   circle.center().x,circle.center().y,circle.radius());
      return false;
    }
  }
    if (sideLength() < circle.radius()) {
      for (int i = 0; i < 3; i++) {
        if (sideLength() + Geom::distance(circle.center(), vertices_[i]) < circle.radius()) {
          std::cout<<"GETTIN ON OUTTA HERE. PEEYEW\n";
          return true;
        }
      }
      std::cout << "Not a single thingy is true?\n";
      return false;
    }
  std::cout<<"We're returning here then??\n";
  return true;
}

bool ReuleauxTriangle::containedWithin(RegularConvexPolygon &polygon) {
  int intersections = 0;
  float radius = sideLength();
  Circle sweep = Circle(Point2D(0,0),0);
  for (Line e : polygon.edges()) {
    for (int i = 0; i < 3; i++) {
      sweep = Circle(vertices_[i],radius);
      if (Geom::intersects(e, sweep)) {
        intersections++;
      }
    }
    if (intersections > 2)
      return false;
    else
    intersections = 0;
  }
  return true;
}

/*
 * A reuleaux triangle is contained within another reuleaux triangle if all
 * vertices of the inner triangle are within a side's length from each arena
 * vertex
 */
bool ReuleauxTriangle::containedWithin(ReuleauxTriangle &rt) {
  float sideLength = rt.sideLength();
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (Geom::distance(this->vertices_[i],rt.vertices()[i]) > sideLength) {
        return false;
      }
    }
    return true;
  }
  throw "Not Implemented";
}
