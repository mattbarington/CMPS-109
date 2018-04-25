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

using std::cout;
using std::string;

#define USAGE "USAGE %s test-file\n"

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

static Circle parseCircle(std::stringstream &stream) {
  string point, radius;
  stream >> point >> radius;
  Point2D p = parsePoint2D(point);
  double r = std::stod(radius);
  Circle c(p,r);
  return c;
}

static void parse(std::string line) {
  std::stringstream stream(line);
  string str;
  stream >> str;
  if (str[0] == '#') //ignore line if it starts with #, ignore it
    return;
  if (str == "Circle") {
    Circle inner = parseCircle(stream);
    cout << "inner circle: Center: ("<<inner.center().x<<","<<inner.center().y;
    cout << ") radius: " << inner.radius() << std::endl;
  }
  stream >> str;
  if (str == "Circle") {
    Circle outer = parseCircle(stream);
    cout << "outer circle: Center: ("<<outer.center().x<<","<<outer.center().y;
    cout << ") radius: " << outer.radius() << std::endl;
  }
  stream >> str;
  bool expected = str == "true";
  cout << (expected ? "contained\n" : "not contained\n");

  string testName;
  stream >> str;


  // stream >> inner >> point >> radius;
  // cout << "inner: " << inner << "P:" << point << " radius: " << radius << "\n";
  // stream >> outer >> point >> radius;
  // cout << "outer: " << outer << "P:" << point << " radius: " << radius << "\n";
}



int main(int argc, char *argv[] ){
    if (argc < 2) {
        printf(USAGE, argv[0]);
        return -1;
    }

    std::cout << "Test file: " << argv[1] << std::endl;
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
