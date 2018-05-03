/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include "cube.h"
#include "sphere.h"
#include "geom.h"
#include "reuleauxtetrahedron.h"


Cube::Cube(Point3D upperFace[4], Point3D lowerFace[4]) {
  for (int i = 0; i < 4; i++) {
    upperFace_[i] = upperFace[i];
    lowerFace_[i] = lowerFace[i];
  }
}

bool Cube::containedWithin(Sphere &sphere) {
  for (int i = 0; i < 4; i++) {
    if (!sphere.containsPoint(upperFace_[i]))
      return false;
    if (!sphere.containsPoint(lowerFace_[i]))
      return false;
  }
  return true;
}
bool Cube::containedWithin(Cube &cube) {
  //be rid of z dimension
  std::vector<Point2D> faceThis;
  std::vector<Point2D> faceThat;
  for (int i = 0; i < 4; i++) {
    faceThis.push_back(Point2D(upperVerts()[i].x, upperVerts()[i].y));
    faceThat.push_back(Point2D(cube.upperVerts()[i].x, cube.upperVerts()[i].y));
  }
  RegularConvexPolygon thisSquare = RegularConvexPolygon(faceThis);
  RegularConvexPolygon thatSquare = RegularConvexPolygon(faceThat);

  if (!thisSquare.containedWithin(thatSquare)) {
    return false;
  }
  //clear vertices
  faceThis.clear();
  faceThat.clear();
  //be rid of x dimension
  for (int i = 0; i < 2; i++) {
    faceThis.push_back(Point2D(upperVerts()[i].y,upperVerts()[i].z));
    faceThis.push_back(Point2D(lowerVerts()[i].y,upperVerts()[i].z));
    faceThat.push_back(Point2D(cube.upperVerts()[i].y,cube.upperVerts()[i].z));
    faceThat.push_back(Point2D(cube.lowerVerts()[i].y,cube.upperVerts()[i].z));
  }
  
  thisSquare = RegularConvexPolygon(faceThis);
  thatSquare = RegularConvexPolygon(faceThat);
  return thisSquare.containedWithin(thatSquare);
}
bool Cube::containedWithin(ReuleauxTetrahedron &rt) {
  float side = rt.sideLength();
  for (int i = 0; i < 4; i++) {
    Sphere range(rt.vertices()[i],side);
    for (int i = 0; i < 4; i++) {
      if (!range.containsPoint(this->upperFace_[i]))
        return false;
      if (!range.containsPoint(this->lowerFace_[i]))
        return false;
    }
  }
  return true;
}
