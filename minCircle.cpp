#include "minCircle.h"

// 316383504	318525185

#define EPSILON 0.00001
#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>       /* time */
#define NUM_OF_POINTS_TO_DEFINE_CIRCLE 3
using namespace std;

Circle findCircle_3P(Point **points) {
    // we will use this method with only 3 points int the array.
    Point *p1 = points[0];
    Point *p2 = points[1];
    Point *p3 = points[2];
    // calculate the x1,y1 and x1^2, yq^@
    float x1 = p1->x;
    float y1 = p1->y;
    float px1 = pow(x1, 2);
    float py1 = pow(y1, 2);

    // calculate the x2,y2 and x2^2, y2^2
    float x2 = p2->x;
    float y2 = p2->y;
    float px2 = pow(x2, 2);
    float py2 = pow(y2, 2);

    // calculate the x3,y3 and x3^2, y3^2
    float x3 = p3->x;
    float y3 = p3->y;
    float px3 = pow(x3, 2);
    float py3 = pow(y3, 2);

    // calculate cx ( based on algoriatm that cx,cy are variables and x1,x2,x3 (and the pows) are flout numbers)
    // note that cx^2 and cy^2 will offset in the equation.
    float cx_part1 =(px1 - px3)*(y1 - y2);
    float cx_part2 =(py1 - py3)*(y1 - y2);
    float cx_part3 =(px2 - px1) * (y1 - y3);
    float cx_part4=(py2 - py1) * (y1 - y3);
    float cx_part5=(x3 - x1) * (y1 - y2);
    float cx_part6=(x2 - x1) * (y1 - y3);


    // calculate cy ( based on algoriatm that cx,cy are variables and x1,x2,x3 (and the pows) are flout numbers)
    // note that cx^2 and cy^2 will offset in the equation.
    float cy_part1 =(px1 - px3)*(x1 - x2);
    float cy_part2 =(py1 - py3)*(x1 - x2);
    float cy_part3 =(px2 - px1)*(x1 - x3);
    float cy_part4=(py2 - py1)*(x1 - x3);
    float cy_part5=(y3 - y1)*(x1 - x2);
    float cy_part6=(y2 - y1)*(x1 - x3);

    float cx = (-0.5) * ( cx_part1+ cx_part2 + cx_part3 +cx_part4 ) /(cx_part5 - cx_part6);
    float pcx = pow(cx, 2);

    float cy = (-0.5) *( cy_part1+ cy_part2 + cy_part3 + cy_part4) /(cy_part5 - cy_part6);
    float pcy = pow(cy, 2);

    // calcuste the c number of the circle , based on the circle math (x-cx)^2 + (y-cy)^2
    float c = -px1 - py1 + (2 * cx * x1) + (2 * cy * y1);
    //calculate the Radius
    float beforeR = pcx + pcy - c;

    float r = sqrt(beforeR);
    Point *center = new Point(cx, cy);
    return Circle(*center, r);
}

double distanceBetween_2Points(Point point_1, Point point_2) {
    float x12 = point_2.x - point_1.x;
    float y12 = point_2.y - point_1.y;
    return sqrt(pow(x12, 2) + pow(y12, 2));
}

Point findMiddlePoint(Point point_1, Point point_2) {
    return Point((point_1.x + point_2.x) / 2, (point_1.y + point_2.y) / 2);
}

bool isPointInCircle(Circle circle, Point point) {
    if (distanceBetween_2Points(circle.center, point) <= circle.radius + EPSILON) { //if in range
        return true;
    }
    return false;
}

bool isPointInCircle_bigger(Circle circle, Point point,float number) {
    if (distanceBetween_2Points(circle.center, point) <= (number*(circle.radius)) + EPSILON) { //if in range
        return true;
    }
    return false;
}

Circle CircleFrom_3_FinalPoints(std::vector<Point> points) {
    Point *pointsArray[NUM_OF_POINTS_TO_DEFINE_CIRCLE];
    pointsArray[0] = &points[0];
    pointsArray[1] = &points[1];
    pointsArray[2] = &points[2];
    return findCircle_3P(pointsArray);
}

Circle findMinCircleAlgorithm(Point **points, std::size_t size, std::vector<Point> finalPoints) {
    if (finalPoints.size() == NUM_OF_POINTS_TO_DEFINE_CIRCLE) {
        return CircleFrom_3_FinalPoints(finalPoints);
    }
    if (size == 0) { //if points is empty
        if (finalPoints.size() == 0)
            return Circle(Point(0,0),0);
        else if (finalPoints.size() == 1)
            return Circle(Point(finalPoints[0].x,finalPoints[0].y),0);
        else if (finalPoints.size() == 2)
            return Circle(findMiddlePoint(finalPoints[0],finalPoints[1]),
                          distanceBetween_2Points(finalPoints[0], finalPoints[1])/2);
        else if (finalPoints.size() == 3)
            return findCircle_3P(points);
    }
    Circle circle = findMinCircleAlgorithm(points, size - 1, finalPoints); //the circle without the last point
    if (isPointInCircle(circle, *(points[size - 1]))) {
        return circle;
    } else {
        finalPoints.push_back(*(points[size - 1]));
        return findMinCircleAlgorithm(points, size - 1, finalPoints);
    }
}

Circle findMinCircle(Point **points, size_t size) {
    return findMinCircleAlgorithm(points, size, std::vector<Point>());

}

