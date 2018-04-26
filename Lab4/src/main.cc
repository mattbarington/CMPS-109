/*
 * Copyright (C) 2018 Matthew Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string> //stod
#include "containable.h"
#include "point.h"
#include "circle.h"
#include "sphere.h"
#include "reuleauxtriangle.h"

using std::cout;
using std::string;

#define USAGE "USAGE %s test-file\n"
//#define DEBUG

static void test(std::string test, bool expected, bool result) {
  std::cout << test << ": " << (expected == result ? "PASS\n" : "FAIL\n");
}

/*
 * Extracts doubles from string formatted "double,double"
 * Source: https://stackoverflow.com/posts/10861816/revisions
 */
static Point2D parsePoint2D(string &str) {
  std::vector<string> result;
  std::stringstream ss(str);
  while( ss.good() ) {
    string substr;
    getline( ss, substr, ',' );
    result.push_back( substr );
  }
  Point2D p(std::stod(result[0]),std::stod(result[1]));
  return p;
}

static Point3D parsePoint3D(string &str) {
  std::vector<string> result;
  std::stringstream ss(str);
  while( ss.good() ) {
    string substr;
    getline( ss, substr, ',' );
    result.push_back( substr );
  }
  Point3D p(std::stod(result[0]),std::stod(result[1]),std::stod(result[2]));
  return p;
}

static Circle* parseCircle(std::stringstream &stream) {
  string point, radius;
  stream >> point >> radius;
  Point2D p = parsePoint2D(point);
  double r = std::stod(radius);
  Circle* c = new Circle(p,r);
  return c;
}

static ReuleauxTriangle* parseReuleauxTriangle(std::stringstream &stream) {
  string point;
  Point2D points[3];
  for (int i = 0; i < 3; i++) {
    stream >> point;
    points[i] = parsePoint2D(point);
    #ifdef DEBUG
    std::cerr << point << " same as ";
    std::cerr << points[i].x << " , " << points[i].y << std::endl;
    #endif
  }
  ReuleauxTriangle* rt = new ReuleauxTriangle(points);
  #ifdef DEBUG
    std::cerr << "Just to double check.. here are all of the rt verts\n";
    for (int i = 0; i < 3; i++) {
        cout << rt->vertices()[i].toString();
    }
    cout << "\nHow'd they come out?\n";
  #endif

  return rt;
}

static Sphere* parseSphere(std::stringstream &stream) {
  string point, radius;
  stream >> point >> radius;
  Point3D p = parsePoint3D(point);
  double r = std::stod(radius);
  Sphere* s =  new Sphere(p,r);
  return s;

}

static void parse2D(string &line) {
  std::stringstream stream(line);
  string str;
  std::vector<Containable2D*> shapes;
  for (int i = 0; i < 2; i++) {
    stream >> str;
    if (str == "Circle")
      shapes.push_back(parseCircle(stream));
    else if (str == "ReuleauxTriangle")
      shapes.push_back(parseReuleauxTriangle(stream));
    // else
    //   shapes.push_back(parsePolygon(stream));
  }

  Circle* ci = dynamic_cast<Circle*> (shapes[0]);
  Circle* co = dynamic_cast<Circle*> (shapes[1]);

  ReuleauxTriangle* rti = dynamic_cast<ReuleauxTriangle*> (shapes[0]);
  ReuleauxTriangle* rto = dynamic_cast<ReuleauxTriangle*> (shapes[1]);

  RegularConvexPolygon* pi = dynamic_cast<RegularConvexPolygon*> (shapes[0]);
  RegularConvexPolygon* po = dynamic_cast<RegularConvexPolygon*> (shapes[1]);

  stream >> str;
  bool expected = str == "true";
  string testName;
  while (stream >> str) { testName += str + " ";}

  if (ci) {
    #ifdef DEBUG
    cout << "We have got an inner circle, folks! : ";
    cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
    #endif
    if (co) {
      #ifdef DEBUG
      cout << "We have got an outer circle, folks! : ";
      cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
      #endif
      test(testName, expected, ci->containedWithin(*co));
    } else if (rto) {
      #ifdef DEBUG
        cout << "We have got an outer reuleauxTriangle, folks!\n";
        for (int i = 0; i < 3; i++) {
          cout << rti->vertices()[i].toString() << " ";
        }
        cout << std::endl;
        test(testName, expected, ci->containedWithin(*rto));
      #endif
      throw "rto Note yet implemented";
    } else if (po) {
      throw "po Note yet implemented";
    } else {
      throw "Something is wrong. No outer 2D shape is matched";
    }
  } else if (rti) {
    #ifdef DEBUG
      cout << "We have got an inner reuleauxTriangle, folks!\n";
      for (int i = 0; i < 3; i++) {
        cout << rti->vertices()[i].toString() << " ";
      }
      cout << std::endl;
    #endif
    if (co) {
      #ifdef DEBUG
      cout << "We have got an inner circle, folks! : ";
      cout << co->center().toString() << " w/ radius " << co->radius() << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*co));
    } else if (rto) {
      throw "rto Note yet implemented";
    } else if (po) {
      throw "po Note yet implemented";
    } else {
      throw "Something is wrong. No outer 2D shape is matched";
    }
  } else if (pi) {
    throw "pi Not yet implemented";
    if (co) {
      #ifdef DEBUG
      cout << "We have got an inner circle, folks! : ";
      cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
      #endif
    } else if (rto) {
      throw "rto Note yet implemented";
    } else if (po) {
      throw "po Note yet implemented";
    } else {
      throw "Something is wrong. No outer 2D shape is matched";
    }
  } else {
    throw "Something is wrong. No 2D inner shape is matched";
  }



  //parse<Shape> allocates memory, which is stored in the shapes vector;
  //here we deallocate the memory, and all dangling pointers fall out of scope.
  for (int i = 0; i < shapes.size(); i++) {
    free(shapes[i]);
  }
  // string testName;
  // getline(stream,testName);
  // bool containment = inner.containedWithin(outer);
  // cout << testName;
  // cout << (containment == expected? " : PASS\n" : " : FAIL\n");

}

static void parse3D(string &line) {
  std::stringstream stream(line);
  string str;
  std::vector<Containable3D*> shapes;
  for (int i = 0; i < 2; i++) {
    stream >> str;
    if (str == "Sphere")
      shapes.push_back(parseSphere(stream));
  }
  Sphere* si = dynamic_cast<Sphere*> (shapes[0]);
  Sphere* so = dynamic_cast<Sphere*> (shapes[1]);



  stream >> str;
  bool expected = str == "true";
  string testName = "";
  while (stream >> str) {
    testName += str + " ";
  }

  if (si) {
    #ifdef DEBUG
    cout << "We've got an inner sphere, folks : ";
    cout << si->center().toString() << " radius: " << si->radius() << std::endl;
    #endif
    if (so) {
      #ifdef DEBUG
      cout << "We've got an outer sphere, folks : ";
      cout << so->center().toString() << " radius: " << so->radius() << std::endl;
      #endif
      bool contained = si->containedWithin(*so);
      cout << testName;
      cout << (contained == expected ? " : PASS\n" : " : FAIL\n");
    }
  }
}

static void parse(string &line) {
  std::stringstream stream(line);
  string str;
  stream >> str;
  if (str == "Circle" || str == "RegularConvexPolygon" || str == "ReuleauxTriangle") {
    try {
      parse2D(line);
    } catch (const char* e) {
      fprintf(stderr, "Couldn't 2D parse: %s\n", e);
    }
  } else if (str == "Sphere" || str == "Cube" || str == "ReuleauxTetrahedron") {
    try {
      parse3D(line);
    } catch (const char* e) {
      fprintf(stderr, "Couldn't 3D parse: %s\n", e);
    }
  } else {
    throw "Invalid Shape Exception: Expected a shape, got something elses";
  }
  return;
}


int main(int argc, char *argv[] ){
    if (argc < 2) {
        printf(USAGE, argv[0]);
        return -1;
    }

  //  std::cout << "Test file: " << argv[1] << std::endl;
    std::string line;
    std::ifstream fin(argv[1]);
    if (!fin) {
      std::cerr << "There was a problem creating a file stream for " << argv[1];
      std::cerr << std::endl;
      return -1;
    }
    while (getline(fin,line)) {
      if (! (line == "" || line[0] == '#'))
        parse(line);
    }
    return 0;
}
