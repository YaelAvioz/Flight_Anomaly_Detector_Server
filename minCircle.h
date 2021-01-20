#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include "minCircle.h"
#include "anomaly_detection_util.h"
#include <assert.h>
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;

// ------------ DO NOT CHANGE -----------

class Circle{
public:
	Point center;
	float radius;
	Circle(Point c,float r):center(c),radius(r){}
};
// --------------------------------------

// Returns the distance between two points
float distance(Point point1, Point point2);

// Returns circle from two given points
Circle circle_from_two_points(Point point1, Point point2);

// Returns circle from three given points
Circle circle_from_three_points(Point point1, Point point2, Point point3);

// Checks if the point is inside or on the boundaries of the circle
bool point_is_inside(Circle circle, Point point);

// Checks if there is a circle made of two point that contains all the given points
Circle better_circle(vector<Point> &Points);

// Return the min circle for vector of points while the max size of the vector is three
Circle min_circle(vector<Point> &Points);

// The Recursive Algorithem to find minimum circle
Circle min_circle_rec(Point **P, vector<Point> R, size_t size);

// Returns the min circle that contains all the points
Circle findMinCircle(Point **points, size_t size);

#endif /* MINCIRCLE_H_ */
