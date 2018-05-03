/** bounds.c
 *      Author: Matthew Ovenden
 *      CruzId: movenden@ucsc.edu
 *      CMPS109-Lab2
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bounds.h"

static const Shape* arena;

/*
 * Intialise a two or thre dimensional space of shape ARENA containing NUMSHAPES
 * SHAPES, all of which may or may not be entirely contained within ARENA.
 */
void setup(Shape *newArena, Shape *shapes[], int numShapes) {
    arena = newArena;
}

static float slope(Point A, Point B) {
  return (B.y - A.y)/(B.x - A.x);
}

/*
 * Returns true if 3 points are ordered in a counter-clockwise orientation
 * Source:
 * http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
 */
static bool ccw(Point A, Point B, Point C) {
  return (C.y - A.y)*(B.x - A.x) > (B.y - A.y)*(C.x - A.x);
}
/*
 * Returns true if two line segments intersect
 * counter-clockwise based intersection algorithm. Source:
 * http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
 */
static bool linesIntersect(Point A, Point B, Point C, Point D) {
  //parallel lines case
  if (slope(A,B) == slope(C,D)) {
    return false;
  }
  return (ccw(A,C,D) != ccw(B,C,D)) && (ccw(A,B,C) != ccw(A,B,D));
}

/*
 * Returns true if polygon p intersects line segment AB
 */
static bool polygonLineIntersect(Polygon p, Point A, Point B) {
  for (int s = 0; s < p.numVertices - 1; s++) {
    if (linesIntersect(A, B, p.vertices[s], p.vertices[s+1])) {
      return true;
    }
  }
  if (linesIntersect(A, B, p.vertices[p.numVertices - 1],p.vertices[0])) {
    return true;
  }
  return false;
}

/*
 * Returns the x-coordinate of the most positive point in p
 */
static float polygonMaxX(Polygon p) {
  float max = p.vertices[0].x;
  for (int i = 1; i < p.numVertices; i++) {
    if (p.vertices[i].x > max)
      max = p.vertices[i].x;
  }
  return max;
}

/*
 * Returns the distance between two points in 3-dimentsions
 * Source:
 * My guy Pythagoras
 */
static float pointPointDistance(Point A, Point B) {
  return sqrt(pow(A.x - B.x, 2.0) + pow(A.y - B.y, 2.0) + pow(A.z - B.z, 2.0));
}

/*
 * Returns perpendicular distance from point C to line A<-->B
 * Source:
 * https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
 */
static float pointLineDistance(Point A, Point B, Point C) {
    return fabs( ((B.y-A.y)*C.x) - ((B.x-A.x)*C.y) + (B.x*A.y) - (B.y*A.x) ) /
                sqrt( pow(B.y-A.y,2.0) + pow(B.x-B.y,2.0) );
}


static void polygonErrExit(int n) {
  fprintf(stderr, "Error in move: Polygon must cannot have %d sides\n", n);
  exit(-1);
}

static bool circleInReuleauxTriangle(Circle* c, ReuleauxTriangle* rt) {
  float sideLength = pointPointDistance(rt->vertices[0],rt->vertices[1]);
  for (int i = 0; i < 3; i++) {
    float d = pointPointDistance(c->center, rt->vertices[i]);
    if (d > sideLength) return false;
  }
  return true;
}

static bool sphereInReuleauxTetrahedron(Sphere* s, ReuleauxTetrahedron* rt) {
  float sideLength = pointPointDistance(rt->vertices[0],rt->vertices[1]);
  for (int i = 0; i < 4; i++) {
    float d = pointPointDistance(s->center, rt->vertices[i]);
    if (d > sideLength) return false;
  }
  return true;
}

/*
 * Move SHAPE to POINT.
 *
 * Return TRUE if SHAPE remains entirely within the space defined by the
 * ARENA parameter of setup(), FALSE otherwise.
 * Note: the movement of shapes to a point is flawed. Given more time
 * further edge cases could be explored, such as multiple movements.
 * This implementation assumes that the initial center weight of the shape is
 * at the origin before movement
 */
bool move(Shape *shape, Point *point) {
  float d;

    switch (arena->type) {
      case CIRCLE :
        switch (shape->type) {
          case CIRCLE :
            //Circle shape in Circle arena
            d = pointPointDistance(*point, ((Circle*) arena)->center);
            return  d + ((Circle*) shape)->radius <= ((Circle*) arena)->radius;
            //end of circle shape in circle arena

          case POLYGON :
            //Polygon shape in Circle arena
            for (int i = 0; i < ((Polygon*) shape)->numVertices; i++) {
                //p is the new location of each vertex while testing
                //this assumes that the original coordinated of the
                //polygon have a center of mass at the origin.
                //Resultingly, this method is inherently flawed, and
                //should be redesigned later given time availability.
              ((Polygon*)shape)->vertices[i].x += point->x;
              ((Polygon*)shape)->vertices[i].y += point->y;
              ((Polygon*)shape)->vertices[i].z += point->z;


              d = pointPointDistance(((Polygon*)shape)->vertices[i],
                              ((Circle*) arena)->center);
              if (d > ((Circle*) arena)->radius)
                return false;
            }
            return true;
              //end of Polygon shape in Circle arena

          case REULEAUX_TRIANGLE :
            //Reuleaux Triangle in a Circle Arena
            // ReuleauxTriangle* rt = ((ReuleauxTriangle*)shape);
            for (int i = 0; i < 3; i++) {
              ((ReuleauxTriangle*)shape)->vertices[i].x += point->x;
              ((ReuleauxTriangle*)shape)->vertices[i].y += point->y;
              ((ReuleauxTriangle*)shape)->vertices[i].z += point->z;
              d = pointPointDistance(
                ((ReuleauxTriangle*)shape)->vertices[i],
                ((Circle*)arena)->center);
              if (d > ((Circle*)arena)->radius) {
                  return false;
              }
            }
            return true;
          case SPHERE :
            break;
          case REULEAUX_TETRAHEDRON :
            break;
        }
        break;
      case POLYGON :
        if (((Polygon*) arena)->numVertices < 3) {
          polygonErrExit(((Polygon*) arena)->numVertices);
        }
        switch (shape->type) {
          case CIRCLE :
            //Circle shape inside of polygon arena
            for (int i = 0; i < ((Polygon*) arena)->numVertices - 1; i++) {
                d = pointLineDistance(((Polygon*) arena)->vertices[i],
                  ((Polygon*) arena)->vertices[i+1],*point);
              if ( d < ((Circle*) shape)->radius) {
                  return false;
              }
            }
            //test the last<-->first edge
            d = pointLineDistance(((Polygon*) arena)->vertices[0],
              ((Polygon*) arena)->vertices[((Polygon*) arena)->numVertices - 1], *point);
            if ( d < ((Circle*) shape)->radius) {
                return false;
            }
            //at this point we know that no line segment intersects the circle
            //return true if circle center is within some polygon bound
            return (point->x < polygonMaxX(*((Polygon*) arena)));
            //end of circle shape inside of polygon arena

          case POLYGON :
            if (((Polygon*) arena)->numVertices < 3) {
              polygonErrExit(((Polygon*) arena)->numVertices);
            }
            //Polygon shape in Polygon arena
            int num_shape_verticies = ((Polygon*) shape)->numVertices;
            Point* shape_verticies = ((Polygon*) shape)->vertices;
            //if any point's x coordinate is beyond the arena's max X coordinate
            // it cannot be contained
            for (int i = 0; i < num_shape_verticies; i++) {
              if (shape_verticies[i].x + point->x > polygonMaxX(*((Polygon*) arena)))
                return false;
            }

            Point p1;
            Point p2;
            //check if the arena intersects with any shape edges
            for (int s = 0; s < num_shape_verticies - 1; s++) {
              p1.x = shape_verticies[s].x + point->x;
              p1.y = shape_verticies[s].y + point->y;
              p1.z = shape_verticies[s].z + point->z;
              p2.x = shape_verticies[s+1].x + point->x;
              p2.y = shape_verticies[s+1].y + point->y;
              p2.z = shape_verticies[s+1].z + point->z;
              if (polygonLineIntersect( *((Polygon*) arena), p1, p2)) {
                return false;
              }
            }
            p1.x = shape_verticies[0].x + point->x;
            p1.y = shape_verticies[0].y + point->y;
            p1.z = shape_verticies[0].z + point->z;
            p2.x = shape_verticies[num_shape_verticies - 1].x + point->x;
            p2.y = shape_verticies[num_shape_verticies - 1].y + point->y;
            p2.z = shape_verticies[num_shape_verticies - 1].z + point->z;
            if (polygonLineIntersect( *((Polygon*) arena), p1, p2)) {
              return false;
            }
            return true;
            //end of Polygon shape in Polygon arena

          case REULEAUX_TRIANGLE :
            //reuleaux triangle in a polygon arena
            break;
          case SPHERE :
            break;
          case REULEAUX_TETRAHEDRON :
            break;
        } //End of Polygon Arena

        break;
      case REULEAUX_TRIANGLE :
        //circle in a reuleaux_triangle arena
        if (shape->type == CIRCLE) {
          Circle* c = (Circle*)shape;
          c->center.x = point->x;
          c->center.y = point->y;
          return circleInReuleauxTriangle(c,(ReuleauxTriangle*) arena);
        }
        break;
      case SPHERE :
        //reuleaux tetrahedron in spherical arena
        if (shape->type == REULEAUX_TETRAHEDRON) {
          for (int i = 0; i < 4; i++) {
            ((ReuleauxTetrahedron*)shape)->vertices[i].x += point->x;
            ((ReuleauxTetrahedron*)shape)->vertices[i].y += point->y;
            ((ReuleauxTetrahedron*)shape)->vertices[i].z += point->z;
            d = pointPointDistance(
              ((ReuleauxTetrahedron*)shape)->vertices[i],
              ((Sphere*)arena)->center);
              if (d > ((Sphere*)arena)->radius) {
                return false;
              }
          }
          return true;
        }
        break;
      case REULEAUX_TETRAHEDRON :
        //sphere in reuleaux tetrahedrone arena
        if (shape->type == SPHERE) {
          ((Sphere*)shape)->center.x = point->x;
          ((Sphere*)shape)->center.y = point->y;
          ((Sphere*)shape)->center.z = point->z;
          return sphereInReuleauxTetrahedron((Sphere*) shape, (ReuleauxTetrahedron*) arena);
        }
        break;
    }
    //shape or arena musn't have matched any known shape
    return false;
}
