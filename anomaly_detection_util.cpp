/*
 * anomaly_detection_util.cpp
 *
 * Author: Yael Avioz,207237421
 */

#include <math.h>
#include "anomaly_detection_util.h"

// returns the average of array X
float avg(float* x, int size){
	float sum = 0; 
	for(int i = 0; i < size; i++)
	{
		sum += x[i];
	}
	return sum/size;
}

// returns the variance of X and Y
float var(float* x, int size){
	float sum = 0;
	float mu = avg(x, size);
	for(int i = 0; i < size; i++)
	{
		sum += pow(x[i], 2);
	}
	return (sum/size)-(pow(mu, 2));
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size){
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += x[i] * y[i];
	}
	sum = sum /size;

	return sum - avg(x, size) * avg(y, size);
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size){
	return cov(x, y, size)/(sqrt(var(x, size))*sqrt(var(y, size)));
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size){
	float a[size]; 
	float b[size];
	for(int i =0; i<size; i++)
	{
		a[i]=points[i]->x;
		b[i]=points[i]->y;
	}
	float x = cov(a, b, size)/var(a, size);
	float y = avg(b, size)-(x*avg(a, size));
	return Line(x,y);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
	Line line = linear_reg(points, size);
	float distance = p.y - line.f(p.x);
	if(distance < 0)
	{
		distance *= (-1);
	}
	return distance;
}

// returns the deviation between point p and the line
float dev(Point p,Line l){
	float distance = p.y-l.f(p.x);
	 if(distance < 0)
	{
		distance *= (-1);
	}
	return distance;
}