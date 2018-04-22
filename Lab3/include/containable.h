/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 */

/******************  DO NOT MODIFY THIS FILE ****************************
 *
 * It is not included in the subission archive ceated by 'make submit' .
 *
 * If you modify it and your code relies on those modifications, your code
 * will not compile in the automated test harness and will be unable to
 * execute any tests.
 *
 * To put it another way, if you modify this file, you will get a big fat
 * ZERO on this lab.
 *
 ************************************************************************/

#ifndef _CONTAINABLE_H_
#define _CONTAINABLE_H_

// C++ requires forward declaration of any cyclically dependent classes
class Circle;
class RegularConvexPolygon;
class ReuleauxTriangle;
class Sphere;
class Cube;
class ReuleauxTetrahedron;

class Containable2D {
    public:
        virtual bool containedWithin(Circle &circle) = 0;
        virtual bool containedWithin(RegularConvexPolygon &polygon) = 0;
        virtual bool containedWithin(ReuleauxTriangle &rt) = 0;
};

class Containable3D {
    public:
        virtual bool containedWithin(Sphere &sphere) = 0;
        virtual bool containedWithin(Cube &cube) = 0;
        virtual bool containedWithin(ReuleauxTetrahedron &rt) = 0;
};

#endif
