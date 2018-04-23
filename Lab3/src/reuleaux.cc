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

bool ReuleauxTriangle::containsPoint(Point2D p) {
  for (int i = 0; i < 3; i++) {                   //for floating point inprecision
    if (Geom::distance(p, vertices_[i]) > sideLength() + 0.000001)
      return false;
  }
  return true;
}

/*
 * All vertices of the reuleaux triangle must be contained within the circle.
 * If the curvature of the triangle is smaller than the curvature of the
 * containing circle AND the center of the circle is outside the triangle
 * then there must be at least one of the reueaux triangle circles must be
 * fully enclosed within the arena circle.
 */
bool ReuleauxTriangle::containedWithin(Circle &circle) {
  for (int i = 0; i < 3; i++) {
    if (!circle.containsPoint(vertices_[i])) {
      return false;
    }
  }
  if (sideLength() < circle.radius() && !this->containsPoint(circle.center())) {
    for (int i = 0; i < 3; i++) {
      if (Geom::distance(circle.center(), vertices_[i]) + sideLength() <= circle.radius())
        return true;
    }
    return false;
  } else {
    return true;
  }
}

/*
 * There is expected to be no more than two intersections between an arena edge
 * and a circle of the reuleaux triangle. If an arena edge intersected with
 * more that two, three, reuleaux circles, the edge would be on the inside
 * of the reuleaux triangle, and the triangle would resultingly not be
 * contained.
 * TL:DR; 3 intersections = not contained.
 */
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
 * vertex.
 */
bool ReuleauxTriangle::containedWithin(ReuleauxTriangle &rt) {
  for (int i = 0; i < 3; i++) {
    if (!rt.containsPoint(this->vertices_[i]))
      return false;
  }
  return true;
}
