/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#ifndef _BOUNDS_H_
#define _BOUNDS_H_

#include <stdbool.h>

typedef struct point_t {
    float x;
    float y;
    float z;
}
Point;

enum shape_type {
    CIRCLE,
    POLYGON,
    REULEAUX_TRIANGLE,
    SPHERE,
    REULEAUX_TETRAHEDRON,
};

#define TYPE enum shape_type type

typedef struct shape_t {
    TYPE;
}
Shape;

typedef struct circle_t {
    TYPE;
    Point center;
    float radius;
}
Circle;

typedef struct polygon_t {
    TYPE;
    Point *vertices;
    int numVertices;
}
Polygon;

typedef struct reuleaux_triangle_t {
    TYPE;
    Point vertices[3];
}
ReuleauxTriangle;

typedef struct sphere_t {
    TYPE;
    Point center;
    float radius;
}
Sphere;

typedef struct reuleaux_tetrahedron_t {
    TYPE;
    Point vertices[4];
}
ReuleauxTetrahedron;

/*
 * Returns a constant string representation of the shape TYPE.
 */
extern const char *getShapeName(enum shape_type type);


extern const Shape *shapes[];

void setup(Shape *arena, Shape *shapes[], int numShapes);

bool move(Shape *shape, Point *point);

#endif
