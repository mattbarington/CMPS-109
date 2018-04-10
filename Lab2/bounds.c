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

bool ccw(float Ax, float Ay, float Bx, float By, float Cx, float Cy) {
  return ((Cy - Ay)*(Bx - Ax) > (Bx - Ay)*(Cx - Ax));
}

// counter-clockwise based intersection algorithm. Source:
// http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
bool linesIntersect(float x1, float y1, float x2, float y2) {
  //parallel lines case
  if ((x1/y1) == (x2/y2)) return false;
  return false;
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

            //Circle shape in circle arena
            d = pointPointDistance(*point, ((Circle*) arena)->center);
            // fprintf(stderr, "arena: %f,%f, radius: %f\n", ((Circle*) arena)->center.x,((Circle*) arena)->center.y,((Circle*) arena)->radius);
            // fprintf(stderr, "shape: %f,%f, radius: %f \n", ((Circle*) shape)->center.x,((Circle*) shape)->center.z,((Circle*) shape)->radius);
            // fprintf(stderr, "distance between radiuses: %.0f\n", distance(((Circle*) shape)->center, ((Circle*) arena)->center));
            // fprintf(stderr, "distance: %f\n", d);
            // fprintf(stderr, "R: %f\n", ((Circle*) arena)->radius);
            // fprintf(stderr, "r: %f\n", ((Circle*) shape)->radius);
            return  d + ((Circle*) shape)->radius < ((Circle*) arena)->radius;
            //end of circle shape in circle arena

          case POLYGON :

            //Circle shape inside of polygon arena
            if (((Polygon*) arena)->numVertices < 3) {
              fprintf(stderr, "Error in move: Polygon must have >=3 sides\n");
              exit(-1);
            }
            for (int i = 0; i < ((Polygon*) arena)->numVertices; i++) {
                if (i == ((Polygon*) arena)->numVertices - 1) {
                  d = pointLineDistance(((Polygon*) arena)->vertices[i],
                      ((Polygon*) arena)->vertices[0],*point);
                } else {
                  d = pointLineDistance(((Polygon*) arena)->vertices[i],
                    ((Polygon*) arena)->vertices[i+1],*point);
                }
                if ( d < ((Circle*) shape)->radius) {
                    return false;
                }
            }
            return true;
            //end of circle shape inside of polygon arena


          case REULEUX_TRIANGLE :
            break;
          case SPHERE :
            break;
          case REULEUX_TETRAHEDRON :
            break;
        }
        break;
      case POLYGON :
        switch (shape->type) {
          case CIRCLE :

            //Polygon shape in Circle arena
            for (int i = 0; i < ((Polygon*) shape)->numVertices; i++) {
              d = pointPointDistance(((Polygon*) shape)->vertices[i],
                                      ((Circle*) shape)->center);
              if (d > ((Circle*) shape)->radius)
                  return false;
            }
            return true;
            //end of Polygon shape in Circle arena

          case POLYGON :
              if (((Polygon*) arena)->numVertices < 3) {
                fprintf(stderr, "Error in move: Polygon must have >=3 sides\n");
                exit(-1);
              }


              //Polygon shape in Polygon arena
              for (int i = 0; i < ((Polygon*) arena)->numVertices; i++) {
                  if (i == ((Polygon*) arena)->numVertices - 1) {
                    d = pointLineDistance(((Polygon*) arena)->vertices[i],
                        ((Polygon*) arena)->vertices[0],*point);
                  } else {
                    d = pointLineDistance(((Polygon*) arena)->vertices[i],
                      ((Polygon*) arena)->vertices[i+1],*point);
                  }
                  if ( d < ((Circle*) shape)->radius) {
                      return false;
                  }
              }
              return true;
              //end of Polygon shape in Polygon arena

          case REULEUX_TRIANGLE :
            break;
          case SPHERE :
            break;
          case REULEUX_TETRAHEDRON :
            break;
        }





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
