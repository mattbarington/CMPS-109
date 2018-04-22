/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include "geom.h"
#include "circle.h"
#include "polygon.h"
#include "reuleauxtriangle.h"

static void success(std::string test, bool expected, bool result) {
  std::cout << test << ": " << (expected == result ? "PASS\n" : "FAIL\n");
}

static void circleInCircle() {
  Circle arena = Circle(Point2D(0,0), 8);
  Circle inner = Circle(Point2D(0,0), 4);
  success("Circle-In-Circle", true, inner.containedWithin(arena));
}

static void circlePartialInCircle() {
  Circle arena = Circle(Point2D(0,0), 8);
  Circle inner = Circle(Point2D(0, 4.001), 4);
  success("Circle-Partially-In-Circle", false, inner.containedWithin(arena));
}

static void circlePartialOutCircle() {
  Circle arena = Circle(Point2D(0,0), 8);
  Circle inner = Circle(Point2D(10, 10), 3);
  success("Circle-Partially-Out-Circle", false, inner.containedWithin(arena));
}

static void circleOutofCircle() {
  Circle arena = Circle(Point2D(0,0), 8);
  Circle inner = Circle(Point2D(13,13), 4);
  success("Circle-Out-Circle",false, inner.containedWithin(arena));
}

static void triangleInCircle() {
  Circle arena = Circle(Point2D(0,0), 120);
  RegularConvexPolygon triangle = RegularConvexPolygon(
      std::vector<Point2D> {Point2D(-5,-5), Point2D(0,5), Point2D(5,-5)});
  success("Triangle-In-Circle-1", true, triangle.containedWithin(arena));
}

static void squaresInSquare() {
  RegularConvexPolygon arena = RegularConvexPolygon(
      std::vector<Point2D> {Point2D(-10,10), Point2D(10,10),
                            Point2D(10,-10), Point2D(-10,-10)});
  for (int i = -1; i <= 1; i += 2) {
    for (int j= -1; j <= 1; j+= 2) {
      RegularConvexPolygon inner = RegularConvexPolygon(
        std::vector<Point2D> {Point2D(i*4, j*9), Point2D(i*9,j*9),
                              Point2D(i*9, j*4), Point2D(i*4, j*4)});
      success("Squares-In-Squares-1", true, inner.containedWithin(arena));
    }
  }
  for (int i = -1; i <= 1; i += 2) {
    for (int j= -1; j <= 1; j+= 2) {
      RegularConvexPolygon inner = RegularConvexPolygon(
        std::vector<Point2D> {Point2D(i*4, j*11), Point2D(i*11,j*11),
                              Point2D(i*11, j*4), Point2D(i*4, j*4)});
      success("Squares-In-Squares-2", false, inner.containedWithin(arena));
    }
  }
}

static void trianglesInSquare() {
  RegularConvexPolygon arena = RegularConvexPolygon(
      std::vector<Point2D> {Point2D(-10,10), Point2D(10,10),
                            Point2D(10,-10), Point2D(-10,-10)});
  for (int i = -1; i <= 1; i += 2) {
    for (int j = -1; j <= 1; j += 2) {
      RegularConvexPolygon inner(
        std::vector<Point2D> {Point2D(i*3,j*8),Point2D(i*8,j*8),Point2D(i*5,j*3)});
      success("Triangles-in-Squares-1", true, inner.containedWithin(arena));
    }
  }
}

static void squareBiggerThanSquares() {
  RegularConvexPolygon inner(
    std::vector<Point2D> {Point2D(-20,-20),Point2D(-20,20),
                          Point2D(20,20),Point2D(20,-20)});
  RegularConvexPolygon outer(
    std::vector<Point2D> {Point2D(-10,-10),Point2D(-10,10),
                          Point2D(10,10),Point2D(10,-10)});
  success("Square-Bigger-Than-Square", false, inner.containedWithin(outer));
}

static void circlesInRect() {
  RegularConvexPolygon arena (
    std::vector<Point2D> {Point2D(-21,11),Point2D(21,11),Point2D(21,-11),Point2D(-21,-11)});
    for (int i = -15; i <= 15; i += 10) {
      for (int j = -5; j <= 5; j += 10) {
        Circle inner(Point2D(i,j), 5);
        success("circles-In-Rectangle-1", true, inner.containedWithin(arena));
      }
    }
  arena =  RegularConvexPolygon(
    std::vector<Point2D> {Point2D(-20,10),Point2D(20,10),Point2D(20,-10),Point2D(-20,-10)});
  for (int i = -15; i <= 15; i += 10) {
    for (int j = -5; j <= 5; j += 10) {
      Circle inner(Point2D(i,j), 5);
      success("circles-In-Rectangle-2", true, inner.containedWithin(arena));
    }
  }
}

static void circlesPartiallyInRectangle() {
  RegularConvexPolygon arena(
    std::vector<Point2D> {Point2D(-20,10),Point2D(20,10),Point2D(20,-10),Point2D(-20,-10)});
  success("Circles-Partially-In-Rectanlge-1", false, Circle(Point2D(16,4),4.01).containedWithin(arena));
  success("Circles-Partially-In-Rectanlge-2", false, Circle(Point2D(-16,4),4.01).containedWithin(arena));
  success("Circles-Partially-In-Rectanlge-3", false, Circle(Point2D(-16,-4),4.01).containedWithin(arena));
  success("Circles-Partially-In-Rectanlge-4", false, Circle(Point2D(16,-4),4.01).containedWithin(arena));
}

static void circlesOutOfOctagon() {
  RegularConvexPolygon oct(std::vector<Point2D> {
    Point2D(-2,4),Point2D(2,4),Point2D(4,2),Point2D(4,-2),Point2D(2,-4),
    Point2D(-2,-4),Point2D(-4,-2),Point2D(-4,2)});
  success("Circles-Out-Of-Octagon-1", false, Circle(Point2D(10,10),7).containedWithin(oct));
  success("Circles-Out-Of-Octagon-2", false, Circle(Point2D(-10,10),7).containedWithin(oct));
  success("Circles-Out-Of-Octagon-3", false, Circle(Point2D(10,-10),7).containedWithin(oct));
  success("Circles-Out-Of-Octagon-4", false, Circle(Point2D(-10,-10),9).containedWithin(oct));
}

static void circleInReuleaux() {
  //0,8,0 -7.6193,-7.6193,0 7.6193,-7.6193,0
  Point2D points[] = {Point2D(0,8), Point2D(-7.6193,-7.6193),Point2D(7.6193,-7.6193)};
  ReuleauxTriangle arena(points);
  success("Circle-In-Reuleaux-1",true,Circle(Point2D(0,0),5).containedWithin(arena));
  success("Circle-In-Reuleaux-2",false,Circle(Point2D(0,0),7).containedWithin(arena));
  success("Circle-In-Reuleaux-3",true,Circle(Point2D(4,0),3).containedWithin(arena));
  success("Circle-In-Reuleaux-4",false,Circle(Point2D(0,7),1).containedWithin(arena));
}

static void squareInReuleaux() {
  Point2D reuleuxPoints[] = {Point2D(0,8.046), Point2D(5.8,-2),Point2D(-5.8,-2)};
  ReuleauxTriangle arena(reuleuxPoints);
  RegularConvexPolygon square = RegularConvexPolygon(std::vector<Point2D> {
            Point2D(-2,4),Point2D(2,4),Point2D(2,0),Point2D(-2,0)});
  success("Square-In-Reuleaux-1", true, square.containedWithin(arena));
  square = RegularConvexPolygon(std::vector<Point2D> {
            Point2D(-5,-5),Point2D(-5,5),Point2D(5,5),Point2D(5,-5)});
  success("Square-In-Reuleaux-2", false, square.containedWithin(arena));
  square = RegularConvexPolygon(std::vector<Point2D> {
            Point2D(-10,-10),Point2D(-10,10),Point2D(10,10),Point2D(10,-10)});
  success("Square-In-Reuleaux-3", false, square.containedWithin(arena));
}

static void triangleInReuleaux() {
  Point2D reuleuxPoints[] = {Point2D(0,8.046), Point2D(5.8,-2),Point2D(-5.8,-2)};
  ReuleauxTriangle arena(reuleuxPoints);
  RegularConvexPolygon triangle = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(0,8.045), Point2D(5.7,-1.9),Point2D(-5.7,-1.9)});
  success("Triangle-In-Reuleaux-1",true,triangle.containedWithin(arena));
  triangle = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(0,6), Point2D(-3,-1),Point2D(3,-1)});
  success("Triangle-In-Reuleaux-2",true,triangle.containedWithin(arena));
  triangle = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(0,8.046), Point2D(5.9,-2),Point2D(-5.9,-2)});
  success("Triangle-In-Reuleaux-3",false,triangle.containedWithin(arena));
  triangle = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(0,8.046), Point2D(5.8,-2),Point2D(-5.8,-2)});
  success("Triangle-In-Reuleaux-4",true,triangle.containedWithin(arena));
}

static void reuleauxInCircle() {
  Circle arena = Circle(Point2D(0,0),100);
  Point2D reuleuxPoints[] = {Point2D(0,8.046), Point2D(5.8,-2),Point2D(-5.8,-2)};
  ReuleauxTriangle inner(reuleuxPoints);
  success("Reuleaux-In-Circle-1",true,inner.containedWithin(arena));
  arena = Circle(Point2D(0,0),12);
  success("Reuleaux-In-Circle-2",true,inner.containedWithin(arena));
  arena = Circle(Point2D(0,8),18);
  success("Reuleaux-In-Circle-3",true,inner.containedWithin(arena));
  arena = Circle(Point2D(8,8), 10);
  success("Reuleaux-In-Circle-4",false,inner.containedWithin(arena));
  arena = Circle(Point2D(2,0),1);
  success("Reuleaux-In-Circle-5",false,inner.containedWithin(arena));
  arena = Circle(Point2D(0,2),7.2);
  success("Reuleaux-In-Circle-6",true,inner.containedWithin(arena));
  arena = Circle(Point2D(0,14),17.03);
  success("Reuleaux-In-Circle-7",false,inner.containedWithin(arena));
  arena = Circle(Point2D(0,60),64);
  success("Reuleaux-In-Circle-8",true,inner.containedWithin(arena));
}

static void reuleauxInSquare() {
  RegularConvexPolygon arena = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(-8,10), Point2D(8,10), Point2D(8,-6),Point2D(-8,-6)});
  Point2D points[] = {Point2D(0,8.046), Point2D(5.8,-2),Point2D(-5.8,-2)};
  ReuleauxTriangle inner(points);
  success("Reuleaux-In-Square-1",true,inner.containedWithin(arena));
  arena = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(-6,6), Point2D(6,6), Point2D(6,-6), Point2D(-6,-6)});
  success("Reuleaux-In-Square-2",false,inner.containedWithin(arena));
  arena = RegularConvexPolygon(std::vector<Point2D> {
    Point2D(-7,-2.5), Point2D(-7,11.5), Point2D(7,11.5), Point2D(7,-2.5)});
  success("Reuleaux-In-Square-3",false,inner.containedWithin(arena));
}

int main(int argc, char *argv[])
{
    circleInCircle();
    circlePartialInCircle();
    circlePartialOutCircle();
    circleOutofCircle();
    triangleInCircle();
    squaresInSquare();
    trianglesInSquare();
    squareBiggerThanSquares();
    circlesInRect();
    circlesPartiallyInRectangle();
    circlesOutOfOctagon();
    circleInReuleaux();
    squareInReuleaux();
    triangleInReuleaux();
    reuleauxInCircle();
    reuleauxInSquare();

}
