/*
 * timeseries.h
 *
 * Author: 318525185, Gavriel Sorek
 */
#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include "timeseries.h"
using namespace std;
class TimeSeries {
    std::vector<std::string> features; //the features
    std::vector<std::vector<double>> dataVectors; //all the data that was in the table
    int amountOfFeatures;
    int dataVectorSize;
public:
    TimeSeries(std::string fileName);

    std::vector<double> getDataVectorByFeatureName(std::string name) const;

    std::unique_ptr<std::vector<double>>
    getDataLineVectorByFeatureNameAndValue(std::string featureName, double value) const;

    std::vector<std::string> getFeaturesVector() const;

    double getFeatureValueByTime(std::string featureName, double time) const;
    int getNumOfFeatures() const;
    int getDataVectorSize() const;
    std::unique_ptr<std::vector<double>> getLineVectorByNumber(int num)  const;
    double getDataByLineAndFeature(std::string feature, int line) const;
    ~TimeSeries();

    TimeSeries(const char *CSVfileName) {
        loadData(CSVfileName);
        //the data vector size (the size in place 0 is the size of all)
        this->dataVectorSize = (this->dataVectors[0].size());
    }

private:
    void loadData(std::string srcFile);

    void getFeatures(std::string &line, std::vector<std::string> &features);

    void addLineOfData(std::string line, std::vector<std::vector<double>> (&vData), int size);

    std::vector<std::vector<double>> getData();

    int getLocationOfFeature(std::string featureName) const;

    int getLocationOfNumberInVectorData(std::vector<double> dataVector, double numberToCheck) const;

    void addLine(std::string line);

    void splitString(std::string line, std::vector<std::string> &strVector, char splitBy);
};
class NotValidDataException : public std::exception {
    std::string message = "the data that gaven to the method is not valid, check your data";
public:
    std::string getMassage() {
        return this->message;
    }
};
#endif /* TIMESERIES_H_ */
