/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include "cube.h"


Cube::Cube(Point3D upperFace[4], Point3D lowerFace[4]) {
  upperFace_ = upperFace;
  lowerFace_ = lowerFace;
}

bool Cube::containedWithin(Sphere &sphere) {
  throw "Not implemented";
}
bool Cube::containedWithin(Cube &cube) {
  throw "Not implemented";
}
bool Cube::containedWithin(ReuleauxTetrahedron &rt) {
  throw "Not Implemented";
}
