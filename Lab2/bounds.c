#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bounds.h"

const Shape* arena;

/*
 * Intialise a two or thre dimensional space of shape ARENA containing NUMSHAPES
 * SHAPES, all of which may or may not be entirely contained within ARENA.
 */
void setup(Shape *newArena, Shape *shapes[], int numShapes) {
    arena = newArena;
}

/*
 * Returns true if 3 points are ordered in a counter-clockwise orientation
 * Source:
 * http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
 */
bool ccw(Point A, Point B, Point C) {
  return (C.y - A.y)*(B.x - A.x) > (B.y - A.y)*(C.x - A.x);
}
/*
 * Returns true if two line segments intersect
 * counter-clockwise based intersection algorithm. Source:
 * http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
 */
bool linesIntersect(Point A, Point B, Point C, Point D) {
  //parallel lines case
  if ((B.x - A.x)/(B.y - A.y) == (D.x - C.x)/(D.y - C.y)) {
    return false;
  }
  return (ccw(A,C,D) != ccw(B,C,D)) && (ccw(A,B,C) != ccw(A,B,D));
}

bool polygonLineIntersect(Polygon p, Point A, Point B) {
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

float polygonMaxX(Polygon p) {
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
float pointPointDistance(Point A, Point B) {
  return sqrt(pow(A.x - B.x, 2.0) + pow(A.y - B.y, 2.0) + pow(A.z - B.z, 2.0));
}

/*
 * Returns perpendicular distance from point C to line A<-->B
 * Source:
 * https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
 */
float pointLineDistance(Point A, Point B, Point C) {
    return fabs( ((B.y-A.y)*C.x) - ((B.x-A.x)*C.y) + (B.x*A.y) - (B.y*A.x) ) /
                sqrt( pow(B.y-A.y,2.0) + pow(B.x-B.y,2.0) );
}


void polygonErrExit(int n) {
  fprintf(stderr, "Error in move: Polygon must cannot have %d sides\n", n);
  exit(-1);
}

/*
 * Move SHAPE to POINT.
 *
 * Return TRUE if SHAPE remains entirely within the space defined by the
 * ARENA parameter of setup(), FALSE otherwise.
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
              Point p;  //p is the new location of each vertex while testing
                        //this assumes that the original coordinated of the
                        //polygon have a center of mass at the origin.
                        //Resultingly, this method is inherently flawed, and
                        //should be redesigned later given time availability.

              p.x = ((Polygon*) shape)->vertices[i].x + point->x;
              p.y = ((Polygon*) shape)->vertices[i].y + point->y;
              p.z = ((Polygon*) shape)->vertices[i].z + point->z;

              d = pointPointDistance(p,((Circle*) arena)->center);
              if (d > ((Circle*) arena)->radius)
                return false;
            }
            return true;
              //end of Polygon shape in Circle arena

          case REULEUX_TRIANGLE :
            //Reuleux Triangle in a Circle Arena
            break;
          case SPHERE :
            break;
          case REULEUX_TETRAHEDRON :
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

          case REULEUX_TRIANGLE :
            break;
          case SPHERE :
            break;
          case REULEUX_TETRAHEDRON :
            break;
        } //End of Polygon Arena

        break;
      case REULEUX_TRIANGLE :
        break;
      case SPHERE :
        break;
      case REULEUX_TETRAHEDRON :
        break;
    }
    printf("Not implemented!\n");
    exit(-1);
}
