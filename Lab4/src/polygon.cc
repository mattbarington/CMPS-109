/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include "geom.h"
#include "polygon.h"
#include "reuleauxtriangle.h"

RegularConvexPolygon::RegularConvexPolygon(std::vector<Point2D> vertices) {
    vertices_ = vertices;
}

std::vector<Point2D> RegularConvexPolygon::vertices() {
    return vertices_;
}

std::vector<Line> RegularConvexPolygon::edges() {
    std::vector<Line> edges;
    for (unsigned int i = 0; i < vertices_.size()-1; i++)
        edges.push_back(Line(vertices_[i],vertices_[i+1]));
    edges.push_back(Line(vertices_[vertices_.size()-1],vertices_[0]));
    return edges;
}

int RegularConvexPolygon::vertexCount() {
    return vertices_.size();
}


/*
 * Returns the maximum X coordinate in the polygon
 */
float RegularConvexPolygon::maxXCoordinate() {
  float max = vertices_[0].x;
  for (Point2D p : vertices_) {
      if (p.x > max)
        max = p.x;
  }
  return max;
}

/*
 * If any point of the polygon is futher from the center of the circle than
 * the radius length, it musn't be in contained within the circle
 */
bool RegularConvexPolygon::containedWithin(Circle &circle) {
    for (Point2D p : vertices_) {
      if (!circle.containsPoint(p))
        return false;
    }
    return true;
}

/*
 * If any edge of the polygon intersect any edge of the arena, there must be
 * partial containment.
 * If the maximum x coordinate of the shape is greater than the maximum x
 * coordinate of the arena, it cannot possibly be contained within the arena
 */
bool RegularConvexPolygon::containedWithin(RegularConvexPolygon &polygon) {
  for (Line e : edges()) {
    for (Line p : polygon.edges()) {
      if (Geom::intersects(e,p)) return false;
    }
  }
  return this->maxXCoordinate() <= polygon.maxXCoordinate();
}

/*
 * Reuleaux triangles are 'equilateral' with constant side length.
 * Every point in the polygon must be within this side length of each vertex
 * in the reuleaux triangle.
 * if not, it cannot be contained
 */
bool RegularConvexPolygon::containedWithin(ReuleauxTriangle &rt) {
    for (Point2D p : this->vertices()) {
      if (!rt.containsPoint(p))
        return false;
  }
  return true;
}
