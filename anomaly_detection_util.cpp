
/*
 * animaly_detection_util.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */
#include <string.h>
#include <iostream>
#include <math.h>
#include "anomaly_detection_util.h"
// return the average of the array x
float avg(float* x, int size){
    if (size == 0) {
        return 0;
    }
    float sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += x[i];
    }
    return (sum / size); //dividing by zero problem
}

// returns the variance of X and Y
float var(float* x, int size){
    if (size == 0)
        return 0;      ////////////////////////////////////// need to check
    double sum = 0; //represent the sum of all the elements in the array
    float dSum = 0; //variable that represent the result of pow 2 of any variable in the array and sum it
    //sum all the variable in the array x
    for (int i = 0; i < size; i++) {
        sum += x[i];
        dSum += (x[i] * x[i]);
    }
    float avg = ((float )1 / size) * sum;
    return (((float )1 / size) * dSum) - (avg * avg); //the formula for variance
}

// returns the covariance of X and Y
float cov(float* x, float* y, int size){
    if (size == 0)
        return 0;
    float avgX = avg(x, size); //average of x
    float avgY = avg(y, size); //average of y
    float sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += (x[i] - avgX) * (y[i] - avgY);
    }
    return sum / size;
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float* x, float* y, int size){
    float num = (sqrt(var(x, size)) * sqrt(var(y, size)));
    if (num == 0)
        return 0;   ////////////////////need to change
    return cov(x, y, size) / num;
}

// performs a linear regression and returns the line equation
Line linear_reg(Point** points, int size){

    float xArray[size];
    float yArray[size];
    for (int i = 0; i < size; i++) { //insert the dots coordinates to an arrays
        xArray[i] = points[i]->x;
        yArray[i] = points[i]->y;
    }
    float divideBy = var(xArray, size);
    if (divideBy == 0)
        return Line(0,0);
    float a = cov(xArray, yArray, size) / divideBy; ///////////////danger dividing by zero
    float b = avg(yArray, size) - a * avg(xArray, size);

    return Line(a, b);;
}

// returns the deviation between point p and the line equation of the points
float dev(Point p,Point** points, int size){
    Line line = linear_reg(points, size); //create the line
    return dev(p ,line);
}

// returns the deviation between point p and the line
float dev(Point p,Line l){
    float yOfPointOnTheLine = l.f(p.x);
    float result = yOfPointOnTheLine - p.y;
    if (result >=0) {
        return result;
    } else
        return -1 * result;
}
Line::Line(const Line &line) noexcept {
    this->a = line.a;
    this->b = line.b;
}
