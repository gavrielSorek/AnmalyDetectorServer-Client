// 316383504	318525185

#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_
#define EPSILON 0.00001
#include <iostream>
#include <math.h>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "anomaly_detection_util.h"

#define NUM_OF_POINTS_TO_DEFINE_CIRCLE 3
using namespace std;

// // ------------ DO NOT CHANGE -----------
//class Point {
//public:
//    float x, y;
//
//    Point(float x, float y) : x(x), y(y) {}
//};

class Circle {
public:
    Point center;
    float radius;
    Circle(Point c, float r) : center(c), radius(r) {}
    Circle(): center(0,0),radius(0){} //default val!
};

// implement

Circle findCircle_3P(Point **points);

double distanceBetween_2Points(Point point_1, Point point_2);

Point findMiddlePoint(Point point_1, Point point_2);

bool isPointInCircle(Circle circle, Point point);

bool isPointInCircle_bigger(Circle circle, Point point,float number);

Circle CircleFrom_3_FinalPoints(std::vector<Point> points);

Circle findMinCircleAlgorithm(Point **points, std::size_t size, std::vector<Point> finalPoints) ;

Circle findMinCircle(Point **points, size_t size) ;

#endif /* MINCIRCLE_H_ */