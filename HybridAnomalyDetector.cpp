/*
 * HybridAnomalyDetector.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */
#include "HybridAnomalyDetector.h"
#define MIN_CORRELATION 0.5
#define DEFAULT_POINT 0
#define DEFAULT_THRESHOLD 1.1

HybridAnomalyDetector::HybridAnomalyDetector() {
    // TODO Auto-generated constructor stub
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

void HybridAnomalyDetector::learnNormal(const TimeSeries &ts) {
    SimpleAnomalyDetector::learnNormal(ts);
    hybridCf.clear(); //clear the old data
    std::vector<correlatedFeatures> mostCorrelation = SimpleAnomalyDetector::getMostCorrelateFeatures(ts);
    std::vector<correlatedFeatures> simpleCorrelation = SimpleAnomalyDetector::getNormalModel(); //all the simple correlation features
    for (int i = 0, j = 0; i < mostCorrelation.size(); ++i) { //push all the correlated features to hybridCf
        if (absVal(mostCorrelation[i].corrlation) > MIN_CORRELATION) { //if there is correlation
            //if already in simpleCorrelation
            if (j < simpleCorrelation.size() &&
                mostCorrelation[i].feature1.compare(simpleCorrelation[j].feature1) == 0 &&
                mostCorrelation[i].feature2.compare(simpleCorrelation[j].feature2) == 0) {
                hybridCf.push_back(simpleCorrelation[j]);
                j++;
            } else { //if not on simpleCorrelation
                int size = ts.getDataVectorSize();
                float xArray[size];
                float yArray[size];
                //fill the arrays with the relevant values
                SimpleAnomalyDetector::copyVectorToArray(ts.getDataVectorByFeatureName(mostCorrelation[i].feature1),
                                                         xArray);
                SimpleAnomalyDetector::copyVectorToArray(ts.getDataVectorByFeatureName(mostCorrelation[i].feature2),
                                                         yArray);
                Point *pointArray[size];
                for (int k = 0; k < ts.getDataVectorSize(); ++k) { //create array of points
                    pointArray[k] = new Point(xArray[k], yArray[k]);
                }
                Circle minCircle = findMinCircle(pointArray, size);
                mostCorrelation[i].radius = minCircle.radius;
                mostCorrelation[i].center = minCircle.center;
                hybridCf.push_back(mostCorrelation[i]);
                for (int k = 0; k < ts.getDataVectorSize(); ++k) { //free all the points
                    delete pointArray[k];
                }
            }
        }
    }

}

vector<correlatedFeatures> HybridAnomalyDetector::getNormalModel() {
    return hybridCf;
}

vector<AnomalyReport> HybridAnomalyDetector::detect(const TimeSeries &ts) {
    std::vector<AnomalyReport> circleAnomalyRep = HybridAnomalyDetector::getAnomalyReportFromCycle(ts);
    std::vector<AnomalyReport> simpleAnomalyRep = SimpleAnomalyDetector::detect(ts);
    //combine the 2 vectors
    std::copy(simpleAnomalyRep.begin(), simpleAnomalyRep.end(), std::back_inserter(circleAnomalyRep));
    return circleAnomalyRep;
}
//return true if found anomaly
bool HybridAnomalyDetector::detectExceptionWithCircleAndPoint(Circle circle, Point point) {
    //the distance between the center and the point
    double distance = sqrt(pow(point.x - circle.center.x, 2) + pow(point.y - circle.center.y, 2));
    if (distance > circle.radius * DEFAULT_THRESHOLD)
        return true;
    return false;
}

//return the anomaly that the minCycle found
std::vector<AnomalyReport>
HybridAnomalyDetector::getAnomalyReportFromCycle(const TimeSeries &ts) {
    std::vector<AnomalyReport> anomalyReport;
    for (int i = 0; i < ts.getDataVectorSize(); i++) { //for every line
        for (int j = 0; j < this->hybridCf.size(); j++) { //for all the correlate members
            if (hybridCf[j].radius >= 0) { //if has circle correlated
                Circle minCircle = Circle(hybridCf[j].center, hybridCf[j].radius);
                double featureData1 = ts.getDataByLineAndFeature(this->hybridCf[j].feature1, i);
                double featureData2 = ts.getDataByLineAndFeature(this->hybridCf[j].feature2, i);
                Point p((float) featureData1, (float) featureData2); //create point of the 2 data
                if (detectExceptionWithCircleAndPoint(minCircle, p)) { //if invalid
                    AnomalyReport report = AnomalyReport(this->hybridCf[j].feature1 + "-" +
                                                         this->hybridCf[j].feature2, i + 1); // the line is i + 1
                    anomalyReport.push_back(report);
                }
            }
        }
    }
    return anomalyReport;
}
