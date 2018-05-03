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
 * Source: David Harrison, CMPS109 --UCSC--
 * Secret Sauce, Slide 34,
 * https://classes.soe.ucsc.edu/cmps109/Spring18/SECURE/8.MultiThreaded2.pdf
 */
static Point2D parsePoint2D(string &str) {
  double x,y;
  std::stringstream vstream(str);
  std::string value;
  if (std::getline(vstream, value, ',')) {
    std::stringstream(value) >> x;
    if (std::getline(vstream, value, ',')) {
      std::stringstream(value) >> y;
      return Point2D(x,y);
    }
  }
throw "invalid point " + str;
}

static Point3D parsePoint3D(string &str) {
  std::vector<string> result;
  std::stringstream ss(str);
  while( ss.good() ) {
    string substr;
    getline( ss, substr, ',' );
    result.push_back( substr );
  }
  try {
    Point3D p(std::stod(result[0]),std::stod(result[1]),std::stod(result[2]));
    return p;
  } catch (const std::invalid_argument &e) {
    throw "Failed to Parse Point3D: invalid stod conversion";
  }
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
    cout << point << " same as ";
    cout << points[i].x << " , " << points[i].y << std::endl;
    #endif
  }
  ReuleauxTriangle* rt = new ReuleauxTriangle(points);
  #ifdef DEBUG
    cout << "Just to double check.. here are all of the rt verts\n";
    for (int i = 0; i < 3; i++) {
        cout << rt->vertices()[i].toString();
    }
    cout << "\nHow'd they come out?\n";
  #endif

  return rt;
}

static RegularConvexPolygon* parsePolygon(std::stringstream &stream) {
  #ifdef DEBUG
  cout << "Entering parsePolygon" << std::endl;
  #endif
  string point;
  std::vector<Point2D> points;
  while (stream >> point) {
    if (isalpha(point[0])) {
      #ifdef DEBUG
      cout << "Putting back " << point;
      #endif
      stream.seekg(-point.length(),stream.cur);
      break;
    }
    #ifdef DEBUG
    cout << "Point just read: " << point << std::endl;
    #endif
    points.push_back(parsePoint2D(point));
  }
  RegularConvexPolygon* poly = new RegularConvexPolygon(points);
  #ifdef DEBUG
  cout << "RegularConvexPolygon P: ";
  for (Point2D p : poly->vertices()) {
    cout << p.toString() << " ";
  }
  cout << std::endl;
  #endif
  return poly;
}

static Sphere* parseSphere(std::stringstream &stream) {
  string point, radius;
  stream >> point >> radius;
  Point3D p = parsePoint3D(point);
  double r = std::stod(radius);
  Sphere* s =  new Sphere(p,r);
  return s;
}

static ReuleauxTetrahedron* parseTetrahedron(std::stringstream &stream) {
  string point;
  Point3D points[4];
  for (int i = 0; i < 4; i++) {
    stream >> point;
    points[i] = parsePoint3D(point);
    #ifdef DEBUG
    cout << "Converted : " << points[i].toString() << std::endl;
    #endif
  }
  ReuleauxTetrahedron* rt = new ReuleauxTetrahedron(points);
  #ifdef DEBUG
    cout << "Just to double check.. here are all of the rt verts\n";
    for (int i = 0; i < 4; i++) {
        cout << rt->vertices()[i].toString();
    }
    cout << "\nHow'd they come out?\n";
  #endif
  return rt;
}

static Cube* parseCube(std::stringstream &stream) {
  string point;
  Point3D upper[4];
  Point3D lower[4];
  for (int i = 0; i < 8; i++) {
    stream >> point;
    if (i < 4) {
      upper[i] = parsePoint3D(point);
      #ifdef DEBUG
      cout << "Converted: " << upper.toString() << std::endl;
      #endif
    } else {
      lower[i - 4] = parsePoint3D(point);
      #ifdef DEBUG
      cout << "Converted: " << lower.toString() << std::endl;
      #endif
    }
  }

  Cube* c = new Cube(upper,lower);
  #ifdef DEBUG
  cout << "Just to double check... here are all of the verts:\n";
  cout << "Upper: ";
  for (int i = 0; i < 4; i++) {
    cout << c->upperVerts()[i].toString() << " ";
  }
  cout << "\nLower: ";
  for (int i = 0; i < 4; i++) {
    cout << c->lowerVerts()[i].toString() << " ";
   }
  #endif
  return c;
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
    else if (str == "RegularConvexPolygon")
      shapes.push_back(parsePolygon(stream));
    else
      throw "Unidentifiable 2D Shape";
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

  if (co) {
    #ifdef DEBUG
    cout << "We have got an outer circle, folks! : ";
    cout << co->center().toString() << " w/ radius " << co->radius() << std::endl;
    #endif
    if (ci) {
      #ifdef DEBUG
      cout << "We have got an inner circle, folks! : ";
      cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
      #endif
      test(testName, expected, ci->containedWithin(*co));
    } else if (rti) {
      #ifdef DEBUG
        cout << "We have got an inner reuleauxTriangle, folks!\n";
        for (int i = 0; i < 3; i++) {
          cout << rti->vertices()[i].toString() << " ";
        }
        cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*co));
    } else if (pi) {
      #ifdef DEBUG
      cout << "We Have got an inner polygon, folk! : ";
      for (Point p : pi->vertices()) {
        cout << p.toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName, expected, pi->containedWithin(*co));
    } else {
      throw "Something is wrong. No 2D inner shape is matched";
    }

  } else if (rto) {
    #ifdef DEBUG
      cout << "We have got an outer reuleauxTriangle, folks!\n";
      for (int i = 0; i < 3; i++) {
        cout << rto->vertices()[i].toString() << " ";
      }
      cout << std::endl;
    #endif
    if (ci) {
      #ifdef DEBUG
      cout << "We have got an inner circle, folks! : ";
      cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
      #endif
      test(testName, expected, ci->containedWithin(*rto));
    } else if (rti) {
      #ifdef DEBUG
        cout << "We have got an inner reuleauxTriangle, folks!\n";
        for (int i = 0; i < 3; i++) {
          cout << rti->vertices()[i].toString() << " ";
        }
        cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*rto));
    } else if (pi) {
      #ifdef DEBUG
      cout << "We Have got an inner polygon, folk! : ";
      for (Point p : pi->vertices()) {
        cout << p.toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName,expected, pi->containedWithin(*rto));
    } else {
      throw "Something is wrong. No 2D inner shape is matched";
    }
  } else if (po) {
      #ifdef DEBUG
      cout << "We Have got an outer polygon, folk! : ";
      for (Point p : po->vertices()) {
        cout << p.toString() << " ";
      }
      cout << std::endl;
      #endif
    if (ci) {
      #ifdef DEBUG
      cout << "We have got an inner circle, folks! : ";
      cout << ci->center().toString() << " w/ radius " << ci->radius() << std::endl;
      #endif
      test(testName, expected, ci->containedWithin(*po));
    } else if (rti) {
      #ifdef DEBUG
        cout << "We have got an inner reuleauxTriangle, folks!\n";
        for (int i = 0; i < 3; i++) {
          cout << rti->vertices()[i].toString() << " ";
        }
        cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*po));
    } else if (pi) {
      #ifdef DEBUG
      cout << "We Have got an inner polygon, folk! : ";
      for (Point p : pi->vertices()) {
        cout << p.toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName, expected, pi->containedWithin(*po));
    } else {
      throw "Something is wrong. No 2D inner shape is matched";
    }
  } else {
    throw "Something is wrong. No 2D inner shape is matched";
  }
  //parse<Shape> allocates memory, which is stored in the shapes vector;
  //here we deallocate the memory, and all dangling pointers fall out of scope.
  for (unsigned int i = 0; i < shapes.size(); i++) {
    free(shapes[i]);
  }
}

static void parse3D(string &line) {
  std::stringstream stream(line);
  string str;
  std::vector<Containable3D*> shapes;
  for (int i = 0; i < 2; i++) {
    stream >> str;
    if (str == "Sphere")
      shapes.push_back(parseSphere(stream));
    else if (str == "Cube")
      shapes.push_back(parseCube(stream));
    else if (str == "ReuleauxTetrahedron")
      shapes.push_back(parseTetrahedron(stream));
    else
      throw "Unidentifiable 3D shape";
  }

  Sphere* si = dynamic_cast<Sphere*> (shapes[0]);
  Sphere* so = dynamic_cast<Sphere*> (shapes[1]);

  ReuleauxTetrahedron* rti = dynamic_cast<ReuleauxTetrahedron*> (shapes[0]);
  ReuleauxTetrahedron* rto = dynamic_cast<ReuleauxTetrahedron*> (shapes[1]);

  Cube* ci = dynamic_cast<Cube*> (shapes[0]);
  Cube* co = dynamic_cast<Cube*> (shapes[1]);


  stream >> str;
  bool expected = str == "true";
  string testName = "";
  while (stream >> str) {
    testName += str + " ";
  }

  if (so) {
    #ifdef DEBUG
    cout << "We've got an outer sphere, folks : ";
    cout << so->center().toString() << " radius: " << so->radius() << std::endl;
    #endif
    if (si) {
      #ifdef DEBUG
      cout << "We've got an inner sphere, folks : ";
      cout << si->center().toString() << " radius: " << si->radius() << std::endl;
      #endif
      test(testName, expected, si->containedWithin(*so));
    } else if (ci) {
      #ifdef DEBUG
      cout << "We've got an inner Cube, Folks! : ";
      for (int i =0; i < 4; i++) {
        cout << ci->upperVerts()[i].toString() << " " << ci->lowerVerts()[i].toString() << " ";
      }
      #endif
      test(testName, expected, ci->containedWithin(*so));
    } else if (rti) {
      #ifdef DEBUG
      cout << "We've got an inner tetrahedron! : ";
      for (int i = 0; i < 4; i++) {
        cout << rti->vertices()[i].toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*so));
    } else {
      throw "Unrecognized Shape for dynamic cast";
    }
  } else if (co) {
    #ifdef DEBUG
    cout << "We've got an outer Cube, Folks! : ";
    for (int i =0; i < 4; i++) {
      cout << co->upperVerts()[i].toString() << " " << co->lowerVerts()[i].toString() << " ";
    }
    #endif
    if (si) {
      #ifdef DEBUG
      cout << "We've got an inner sphere, folks : ";
      cout << si->center().toString() << " radius: " << si->radius() << std::endl;
      #endif
      test(testName, expected, si->containedWithin(*co));
    } else if (ci) {
      #ifdef DEBUG
      cout << "We've got an inner Cube, Folks! : ";
      for (int i =0; i < 4; i++) {
        cout << ci->upperVerts()[i].toString() << " " << ci->lowerVerts()[i].toString() << " ";
      }
      #endif
      test(testName, expected, ci->containedWithin(*co));
    } else if (rti) {
      #ifdef DEBUG
      cout << "We've got an inner tetrahedron! : ";
      for (int i = 0; i < 4; i++) {
        cout << rti->vertices()[i].toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*co));
    } else {
      throw "Unrecognized Shape for dynamic cast";
    }
  }
  else if (rto) {
    #ifdef DEBUG
    cout << "We've got an outer tetrahedron! : ";
    for (int i = 0; i < 4; i++) {
      cout << rto->vertices()[i].toString() << " ";
    }
    cout << std::endl;
    #endif
    if (si) {
      #ifdef DEBUG
      cout << "We've got an inner sphere : ";
      cout << sphere->center().toSting() << " radius: " << sphere.radius() << std::endl;
      #endif
      test(testName, expected, si->containedWithin(*rto));
    } else if (ci) {
      #ifdef DEBUG
      cout << "We've got an inner Cube, Folks! : ";
      for (int i =0; i < 4; i++) {
        cout << ci->upperVerts()[i].toString() << " " << ci->lowerVerts()[i].toString() << " ";
      }
      #endif
      test(testName, expected, ci->containedWithin(*rto));
    } else if (rti) {
      #ifdef DEBUG
      cout << "We've got an inner tetrahedron! : ";
      for (int i = 0; i < 4; i++) {
        cout << rti->vertices()[i].toString() << " ";
      }
      cout << std::endl;
      #endif
      test(testName, expected, rti->containedWithin(*rto));
    } else {
      throw "Unrecognized Shape for dynamic cast";
    }
  }
  for (unsigned int i = 0; i < shapes.size(); i++) {
    free(shapes[i]);
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

    std::string line;
    std::ifstream fin(argv[1]);
    if (!fin) {
      std::cerr << "There was a problem creating a file stream for " << argv[1];
      std::cerr << std::endl;
      return -1;
    }
    while (getline(fin,line)) {
      if (! (line == "" || line[0] == '#'))
        try {
          parse(line);
        } catch (const char* e) {
          fprintf(stderr, "Failed to Parse: %s\n", e);
        }
    }
    return 0;
}
