/*
 * HybridAnomalyDetector.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
private:
    vector<correlatedFeatures> hybridCf = std::vector<correlatedFeatures>();
    void updateHybridCorrelationVector (vector<correlatedFeatures> cf, const TimeSeries &ts);
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
	void learnNormal(const TimeSeries &ts) override;
	vector<AnomalyReport> detect(const TimeSeries &ts) override;
    bool detectExceptionWithCircleAndPoint(Circle circle, Point point);
    vector<correlatedFeatures> getNormalModel() override;
    vector<AnomalyReport> getAnomalyReportFromCycle(const TimeSeries &ts);
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
