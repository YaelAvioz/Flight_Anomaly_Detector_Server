/*
* minCircle.h
*
* Author: Yael Avioz, 207237421
*
* I fixed my original code based on the published soultion in the Moodle.
*/

#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include "anomaly_detection_util.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

// ------------ DO NOT CHANGE -----------
class Circle
{
public:
	Point center;
	float radius;
	Circle(Point c, float r) : center(c), radius(r) {}
};
// --------------------------------------

// Returns the distance between two points
float distance(Point point1, Point point2)
{
	float x = powf((point1.x - point2.x), 2.0);
	float y = powf((point1.y - point2.y), 2.0);
	return sqrt(x + y);
}

// Returns circle from two given points
Circle circle_from_two_points(Point point1, Point point2)
{
	float radius = distance(point1, point2) / 2;
	return Circle(Point((point1.x + point2.x) / 2, (point1.y + point2.y) / 2), radius);
}

// Returns circle from three given points
Circle circle_from_three_points(Point point1, Point point2, Point point3){
	
	Point mAB((point1.x+point2.x)/2 , (point1.y+point2.y)/2);
	float slopAB = (point2.y - point1.y) / (point2.x - point1.x); 
	float pSlopAB = - 1/slopAB; 

	Point mBC((point2.x+point3.x)/2 , (point2.y+point3.y)/2);
	float slopBC = (point3.y - point2.y) / (point3.x - point2.x);
	float pSlopBC = - 1/slopBC; 
	
	float x = (- pSlopBC*mBC.x + mBC.y + pSlopAB*mAB.x - mAB.y) / (pSlopAB - pSlopBC);
	float y = pSlopAB * (x - mAB.x) + mAB.y;
	
	return Circle(Point(x,y),distance(Point(x,y),point1));
}

// Checks if the point is inside or on the boundaries of the circle
bool point_is_inside(Circle circle, Point point)
{
	if (distance(circle.center, point) <= circle.radius)
	{
		return true;
	}
	return false;
}

// Checks if there is a circle made of two point that contains all the given points
Circle better_circle(vector<Point> &Points)
{
	// This loop checks if there is minimum circle made by only two points
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 3; j++)
		{
			// Build the circle
			Circle circle = circle_from_two_points(Points[i], Points[j]);

			// Check if this circle is valid by checkig that all the points are inside the circle
			for (int index = 0; index < 3; index++)
			{
				// In case there is no better circle return "empty" circle
				if (!(point_is_inside(circle, Points[index])))
				{
					return Circle(Point(0, 0), 0);
				}
			}
			// In case all the points inside the circle return the new circle
			return circle;
		}
	}
	return Circle(Point(0,0),0);
}

// Return the min circle for vector of points while the max size of the vector is three
Circle min_circle(vector<Point> &Points)
{
	// In case the vector is empty
	if (Points.size() == 0)
	{
		return Circle(Point(0, 0), 0);
	}

	// In case the vector has only one point - Returns a circle from one point
	else if (Points.size() == 1)
	{
		return Circle(Points[0], 0);
	}

	// In case the vector contains two points - Returns a circle that intersects two points
	else if (Points.size() == 2)
	{
		return circle_from_two_points(Points[0], Points[1]);
	}

	// In case the vector contains three points
	else if (Points.size() == 3)
	{
		// Check if there is circle made from two points and contains all the three given points
		Circle new_circle = better_circle(Points);

		// If the radius of the new circle is 0 thats means that there is no better circle
		if (new_circle.radius == 0)
		{
			return circle_from_three_points(Points[0], Points[1], Points[2]);
		}
		return new_circle;
	}
	
	return Circle(Point(0,0),0);
}

// The Recursive Algorithem to find minimum circle
Circle min_circle_rec(Point **P, vector<Point> R, size_t size)
{
	if (size == 0 || R.size() == 3)
	{
		return min_circle(R);
	}

	// Pick a random point
	int index = rand() % size;
	Point point(P[index]->x, P[index]->y);
	swap(P[index], P[size - 1]);

	Circle circle = min_circle_rec(P, R, size - 1);

	if (point_is_inside(circle, point))
	{
		return circle;
	}
	R.push_back(point);

	return min_circle_rec(P, R, size - 1);
}

// Returns the min circle that contains all the points
Circle findMinCircle(Point **points, size_t size)
{
	return min_circle_rec(points, {}, size);
}

#endif /* MINCIRCLE_H_ */