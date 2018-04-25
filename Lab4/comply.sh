#!/bin/bash

#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

ok=true

missing() {
    printf "MISSSING %-12s %s\n" $1 "$2"
    ok=false
}

missing_header() {
    missing "HEADER" $1
}

missing_constructor() {
    missing "CONSTRUCTOR" $1
}

for header in reuleauxtriangle.h sphere.h cube.h reuleauxtetrahedron.h 
do
    ls include/$header 1>/dev/null 2>/dev/null
    if [[ "$?" != "0" ]]
    then
        missing_header $header
        ok=false
    fi
done

echo "ReuleauxTriangle(Point2D vertices[3])" > const.dat
echo "Sphere(Point3D center, double radius)" >> const.dat
echo "Cube(Point3D upperFace[4], Point3D lowerFace[4])" >> const.dat
echo "ReuleauxTetrahedron(Point3D vertices[4])" >> const.dat

IFS=$'\n'
for constructor in $(cat const.dat)
do
    grep -F "$constructor" include/*.h 1>/dev/null 2>/dev/null
    if [[ "$?" != "0" ]]
    then
        missing_constructor $constructor
    fi
done
rm const.dat

if [[ "$ok" != "true" ]]
then
    exit -1
fi
exit 0
