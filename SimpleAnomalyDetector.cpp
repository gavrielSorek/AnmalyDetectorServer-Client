/*
 * SimpleAnomalyDetector.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */
#define DEFAULT_RADIUS -1
#define DEFAULT_POINT 0
#define DEFAULT_THRESHOLD 1.1

#include "SimpleAnomalyDetector.h"

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
}

//learn the normal model, and update the members accordingly.
void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    float minCorrelation = SimpleAnomalyDetector::getCorrelationVal();
    std::vector<correlatedFeatures> mostCorrelate = SimpleAnomalyDetector::getMostCorrelateFeatures(ts);
    std::vector<correlatedFeatures> correlateFeatures = std::vector<correlatedFeatures>();
    for (int i = 0; i < mostCorrelate.size(); ++i) { //push all the correlate features to correlateFeatures
        if (absVal(mostCorrelate[i].corrlation) >= minCorrelation) {//if there is correlation
            cf.push_back(mostCorrelate[i]);
        }
    }
}

//detect anomaly and return vectors with the details
std::vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    std::vector<AnomalyReport> anomalyRep = std::vector<AnomalyReport>();
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //for every line
        for (int j = 0; j < this->cf.size(); j++) { //for all the correlate members
            double featureData1 = ts.getDataByLineAndFeature(this->cf[j].feature1, i);
            double featureData2 = ts.getDataByLineAndFeature(this->cf[j].feature2, i);
            Point p((float) featureData1, (float) featureData2); //create point of the 2 data
            if (detectExceptionWithLineAndPoint( //if invalid
                    this->cf[j].lin_reg, p, this->cf[j].threshold)) {
                AnomalyReport report = AnomalyReport(this->cf[j].feature1 + "-" + this->cf[j].feature2,
                                                     i + 1); // the line is i + 1
                anomalyRep.push_back(report);
            }
        }
    }
    return anomalyRep;
}

//calculate the correlation value between to features and returns it.
float SimpleAnomalyDetector::CorrlationValue(std::string firstFeatureName, std::string secFeatureName,
                                             const TimeSeries &ts) const {
    std::vector<double> dataVector1 = ts.getDataVectorByFeatureName(firstFeatureName);
    std::vector<double> dataVector2 = ts.getDataVectorByFeatureName(secFeatureName);
    int size = dataVector1.size();
    float firstArray[size];
    float secArray[size];
    copyVectorToArray(dataVector1, firstArray);
    copyVectorToArray(dataVector2, secArray);
    return pearson(firstArray, secArray, size);
}

//copy vector to float array
float *SimpleAnomalyDetector::copyVectorToArray(std::vector<double> vector, float *arrayToCopyTo) const {
    for (int i = 0; i < (vector).size(); ++i) {
        arrayToCopyTo[i] = (float) (vector)[i];
    }
    return arrayToCopyTo;
}

//calculate the absolute value of a gaven number , and returns it
float SimpleAnomalyDetector::absVal(float num) const {
    if (num >= 0) {
        return num;
    }
    return -1 * num; //like abs function
}

//return line of 2 features
Line
SimpleAnomalyDetector::getLineFromFeatures(std::string feature1, std::string feature2, TimeSeries const &ts) const {
    std::vector<double> vector1 = ts.getDataVectorByFeatureName(feature1);
    std::vector<double> vector2 = ts.getDataVectorByFeatureName(feature2);
    Point *pArray[ts.getDataVectorSize()];
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //create all the points
        pArray[i] = new Point(vector1[i], vector2[i]); //create the points in the memory that allocated
    }
    Line line_reg = linear_reg(pArray, ts.getDataVectorSize());
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //delete all the points
        delete pArray[i];
    }
    return line_reg;
}

//return the Threshold of 2 features.
float
SimpleAnomalyDetector::getThresholdOfFeatures(std::string feature1, std::string feature2, TimeSeries const &ts,
                                              Line line) const {
    std::vector<double> vector1 = ts.getDataVectorByFeatureName(feature1);
    std::vector<double> vector2 = ts.getDataVectorByFeatureName(feature2);
    Point *pArray[ts.getDataVectorSize()];
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //create all the points
        pArray[i] = new Point(vector1[i], vector2[i]); //create the points in the memory that allocated
    }
    float maxThreshHold = 0;
    for (int i = 0; i < ts.getDataVectorSize(); ++i) {
        float distance = dev(*(pArray[i]), line);
        if (distance > maxThreshHold) {
            maxThreshHold = distance;
        }
    }
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //delete all the points
        delete pArray[i];
    }
    return maxThreshHold;
}

//detect exception accordingly to gaven line and point and Threshold.
bool SimpleAnomalyDetector::detectExceptionWithLineAndPoint(Line line, Point point, float threshHold) const {
    if (dev(point, line) > threshHold) {
        return true;
    }
    return false;
}

//return all the pairs most correlate features
std::vector<correlatedFeatures> SimpleAnomalyDetector::getMostCorrelateFeatures(const TimeSeries &ts) {
    std::vector<correlatedFeatures> mostCorrelate = std::vector<correlatedFeatures>();
    int numOfFeatures = ts.getNumOfFeatures();
    std::vector<std::string> features = ts.getFeaturesVector(); //the features name vector
    for (int i = 0; i < numOfFeatures; i++) { //for each feature
        float maxCorrelation = 0;
        int jLocationOfMaxCorrelation = -1; //default if stay -1 no correlation
        for (int j = i + 1; j < numOfFeatures; j++) {
            float correlation = SimpleAnomalyDetector::CorrlationValue(features[i], features[j], ts);
            if (maxCorrelation < SimpleAnomalyDetector::absVal(correlation)) {
                maxCorrelation = SimpleAnomalyDetector::absVal(correlation);
                jLocationOfMaxCorrelation = j; //the j idx of max correlation with i
            }
        }
        if (jLocationOfMaxCorrelation == -1)  //if there isn't any correlation
            continue;
        float correlation = SimpleAnomalyDetector::CorrlationValue(features[i],
                                                                   features[jLocationOfMaxCorrelation], ts);

        Line line = getLineFromFeatures(features[i], features[jLocationOfMaxCorrelation], ts);
        float threshold =
                getThresholdOfFeatures(features[i], features[jLocationOfMaxCorrelation], ts, line) *
                DEFAULT_THRESHOLD;
        mostCorrelate.push_back(correlatedFeatures(features[i], features[jLocationOfMaxCorrelation], correlation,
                                                   line, threshold, DEFAULT_RADIUS,
                                                   Point(DEFAULT_POINT, DEFAULT_POINT)));
    }
    return mostCorrelate;

}


SimpleAnomalyDetector::SimpleAnomalyDetector() {
}