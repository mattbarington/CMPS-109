/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

 #include "reuleauxtetrahedron.h"

ReuleauxTetrahedron::ReuleauxTetrahedron(Point3D vertices[4]) {
  Point3D verts[4];
  for (int i = 0; i < 4; i++) {
    verts[i] = vertices[i];
  }
  vertices_ = verts;
}

bool ReuleauxTetrahedron::containedWithin(Sphere& sphere) {
  throw "Not Implemented";
}

bool ReuleauxTetrahedron::containedWithin(Cube& cube) {
  throw "Not Implemented";
}

bool ReuleauxTetrahedron::containedWithin(ReuleauxTetrahedron& rt) {
  throw "Not Implemented";
}
