
/*
 * SimpleAnomalyDetector.h
 *
 * Author: 318525185, Gavriel Sorek
 */
#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures {
    string feature1, feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
    float radius;
    Point center;
public:
    correlatedFeatures(std::string feature1, std::string feature2, float corrlation, Line line_reg,
                       float threshold, float radius, Point center) :
            feature1(feature1), feature2(feature2), corrlation(corrlation), lin_reg(line_reg), threshold(threshold),
            radius(radius), center(center) {}
};

class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
    float correlationValue = 0.9; //default
    vector<correlatedFeatures> cf;
public:
    void setCorrelationValue(float val) {
        if (val > 1 || val < 0) //not valid
            return; ////problem
        this->correlationValue = val;
    }
    float getCorrelationVal() {
        return this->correlationValue;
    }
    SimpleAnomalyDetector();

    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries &ts);

    virtual vector<AnomalyReport> detect(const TimeSeries &ts);

    virtual vector<correlatedFeatures> getNormalModel() {
        return cf;
    }

    float CorrlationValue(std::string firstFeatureName, std::string secFeatureName, const TimeSeries &ts) const;

    float absVal(float num) const;

    Line getLineFromFeatures(std::string feature1, std::string feature2, const TimeSeries &ts) const;

    bool detectExceptionWithLineAndPoint(Line line, Point point, float threshHold) const;

    vector<correlatedFeatures> getMostCorrelateFeatures(const TimeSeries &ts);

    float *copyVectorToArray(std::vector<double> vector, float *arrayToCopy) const;

private:


    float getThresholdOfFeatures(string feature1, string feature2, const TimeSeries &ts, Line line) const;
};


#endif /* SIMPLEANOMALYDETECTOR_H_ */
