/*
 * timeseries.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */
#include "timeseries.h"
#include <string.h>
//update the class members according to the data in srcFile.
void TimeSeries::loadData(std::string srcFile) { //extract all the data
    std::ifstream file(srcFile);
    std::string line;
    if (file.fail()) { //if failed to open the file
        exit(1);
    }
    std::getline(file, line); //line of features
    TimeSeries::getFeatures(line, this->features);
    this->amountOfFeatures = this->features.size(); //update the nu, of features accordingly to the features
    for (int i = 0; i < this->amountOfFeatures; ++i) { //add all data vectors
       this->dataVectors.push_back(std::vector<double>());
    }
    while (std::getline(file, line)) {
        if (line.compare("") == 0) //ignore from empty lines
            continue;
        //add data line to vData
        addLineOfData(line, this->dataVectors, amountOfFeatures);
    }
    file.close();
}
//add line of data to vector of data
void TimeSeries::addLineOfData(std::string line, std::vector<std::vector<double>> (&vData), int size) {
    std::vector<std::string> newDataVector;
    splitString(line, newDataVector, ','); //add the data as string
    for (int i = 0; i < size; i++) {
        (vData[i]).push_back(std::stod((newDataVector[i]))); //add the number in the data to the vector
    }
}
// extract all the features from the file
void TimeSeries::getFeatures(std::string &line, std::vector<std::string> &features) {
    TimeSeries::splitString(line, features, ',');
}
//split string according to gaven char
void TimeSeries::splitString(std::string line, std::vector<std::string> &strVector, char splitBy) {
    char charArray[line.size() + 1];
    strcpy(charArray, line.c_str()); //copy the line to array of chars
    std::string word = "";
    for (char  ch: charArray) {
        if ((ch != '\n') && (ch != '\0')) { //if not the end of the line
            if (ch == splitBy) {
                std::string newStr = std::string(word); //copy word to new str
                strVector.push_back(newStr);
                word = "";
            } else {
                word += ch;
            }
        }
    }
    std::string newStr = std::string(word);  //copy the last word
    strVector.push_back(newStr);
}
//return data vector
std::vector<std::vector<double>> TimeSeries::getData() {
    return this->dataVectors;
}
//return feature vector
std::vector<std::string> TimeSeries::getFeaturesVector() const{
    return this->features;
}
//return data vector according to gaven feature
std::vector<double> TimeSeries::getDataVectorByFeatureName(std::string name) const{
    int location = getLocationOfFeature(name);
    return (this->dataVectors[location]);
}
//return allocate vector with the requested line
std::unique_ptr<std::vector<double>> TimeSeries::getDataLineVectorByFeatureNameAndValue(std::string featureName, double value) const {
    auto lineVector = std::unique_ptr<std::vector<double>>(new std::vector<double>);
    int locationOfFeature = getLocationOfFeature(featureName);
    int locationOfLine = getLocationOfNumberInVectorData(this->dataVectors[locationOfFeature], value);
    for (int i = 0; i < this->amountOfFeatures; ++i) { //enter all the line to the vector
         lineVector->push_back(this->dataVectors[i][locationOfLine]);
    }
    return std::move(lineVector);
}
//return location of feature in features vector
int TimeSeries::getLocationOfFeature(std::string featureName) const{
    int location = 0;
    for (int i = 0; i < this->amountOfFeatures; i++) {
        if (!(this->features[i]).compare(featureName)) { //if the name equal to the feature
            location = i; //the location of the feature is i
            break;
        }
    }
    return location;
}
//return location of number in the data vector
int TimeSeries::getLocationOfNumberInVectorData(std::vector<double> dataVector, double numberToCheck) const{
    int location = 0;
    for (int i = 0; i < dataVector.size(); i++) {
        if (dataVector[i] == numberToCheck) { //if the name equal to the feature  ///////////////////equal
            location = i; //the location of the feature is i
            break;
        }
    }
    return location;
}
//return the feature value accordingly to the time that gaven
double TimeSeries::getFeatureValueByTime(std::string featureName, double time)  const{
    int timeLocation = TimeSeries::getLocationOfFeature("Time");
    int featureRequestedLocation = TimeSeries::getLocationOfFeature(featureName);
    int dataLocationInTheVector =
            TimeSeries::getLocationOfNumberInVectorData(this->dataVectors[timeLocation], time);
    return (this->dataVectors[featureRequestedLocation])[dataLocationInTheVector];
}
TimeSeries::~TimeSeries() {

}
//return the amount of features
int TimeSeries::getNumOfFeatures() const{
    return this->amountOfFeatures;
}
//return the size of the data vector
int TimeSeries::getDataVectorSize() const{
    return this->dataVectorSize;
}
/**
 * return the line vector accordingly to the number
 * the rows starts from index 0
 * danger who use this function need to release the memory!!!!!!.
 */
std::unique_ptr<std::vector<double>> TimeSeries::getLineVectorByNumber(int num)  const {
    if (num >= this->dataVectorSize) {
        throw NotValidDataException(); //need to throw exception
    }
    auto lVector = std::unique_ptr<std::vector<double>>(new(std::vector<double>));
    for (int  i = 0; i < this->amountOfFeatures; i++) { //enter all the line to the line vector
        lVector->push_back(this->dataVectors[i][num]);
    }
    return std::move(lVector);
}
/*
 * return data according to line and feature name
 */
double TimeSeries::getDataByLineAndFeature(std::string feature, int line) const{
    int featureLocation = getLocationOfFeature(feature);
    return (this->dataVectors[featureLocation])[line];
}