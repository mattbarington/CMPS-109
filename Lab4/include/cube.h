/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

 #ifndef _CUBE_H_
 #define _CUBE_H_

 #include "point.h"
 #include "containable.h"

class Cube : public Containable3D {
  private:
    Point3D upperFace_[4];
    Point3D lowerFace_[4];

  public:
    Cube(Point3D upperFace[4], Point3D lowerFace[4]);
    Point3D* upperVerts() {return upperFace_;}
    Point3D* lowerVerts() {return lowerFace_;}
    bool containedWithin(Sphere &sphere);
    bool containedWithin(Cube &cube);
    bool containedWithin(ReuleauxTetrahedron &rt);
};


 #endif
