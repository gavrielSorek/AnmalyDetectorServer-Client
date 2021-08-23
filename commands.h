//
// Created by Gavriel on ID 318525185.
//
#ifndef COMMANDS_H_
#define COMMANDS_H_
#include<iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "HybridAnomalyDetector.h"
#include <regex>
#include <set>
#define BUFFER_SIZE 1
#define NOT_VAL -1
#include <sys/socket.h>
//#include <winsock2.h>
#include<iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "HybridAnomalyDetector.h"
#include <regex>
#include <set>

#define NOT_VAL -1
using namespace std;

class DefaultIO {
public:
    virtual string read() = 0;

    virtual void write(string text) = 0;

    virtual void write(float f) = 0;

    virtual void read(float *f) = 0;

    virtual void uploadFile(std::string dataFile, std::string fileName) { //not supposed to use
        std::ofstream file;
        file.open(fileName);
        file << dataFile;
        file.close();
    }

    virtual std::string downloadFile(std::string fileName) {
        std::string data = "";
        std::ifstream file(fileName);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) { //read all the file and put in data
                data += data;
            }
            file.close();
        } else { //if error
            exit(1);
        }
    }

    virtual ~DefaultIO() {}

    // you may add additional methods here
};
class SocketIO: public DefaultIO {
    int clientId;
public:
    SocketIO(int clientId): clientId(clientId) {}
    //write to file the string: fileString.
    void write(std::string txt) override {
        char buffer[txt.length() + 1]; //temporary buffer
        std::strcpy(buffer, txt.c_str());
        send(clientId, buffer, strlen(buffer), 0); //send the string
    }
    std::string read() override {
        std::string retStr;
        char buffer[BUFFER_SIZE];
        retStr = "";
        while(retStr.find("\n") == std::string::npos) { //while not end od line , continue reading
            bzero(buffer, BUFFER_SIZE); //clean the buffer
            //memset(buffer, 0,  sizeof (buffer));
            recv(clientId, buffer, BUFFER_SIZE,0);
            //printf("buffer 2 : %s", buffer);
            retStr += std::string (buffer);
        }
        retStr = retStr.substr(0, retStr.size() - 1); //cut the \n
        return retStr;
    }
    //write float number
    void write(float f) {
        std::ostringstream num;
        num << f;
        std::string s(num.str());
        write(s);
    }
    //read float number
    void read(float* f) {
        std::string numStr = read(); //read the number as string
        float num = ::atof(numStr.c_str()); //convert the number to float
        *f =num;
    }
};
// you may add here helper classes
class SharedData {
public:
    std::vector<AnomalyReport> anomalyVec;
    SimpleAnomalyDetector *anomalyDetector = nullptr;
    int numOfStepsInTestFile;

    ~SharedData() {
        if (anomalyDetector != nullptr) //free the anomaly detector
            delete anomalyDetector;
    }
};

class Command {
private:
    std::string description;
protected:
    DefaultIO *dio;
public:
    Command(DefaultIO *dio, std::string description) : dio(dio), description(description) {}

    virtual ~Command() {
    }

    virtual void execute() = 0;

    std::string getDescription() {
        return this->description;
    }
};

// implement here your command classes

class UploadCommand : public Command {
    std::string fileType;
    std::string fileName;
public:
    void execute() override {
        uploadCSVFile();
    }

    virtual ~UploadCommand() {}

    UploadCommand(DefaultIO *IO, std::string fileType, std::string fileName) : Command(IO,
                                                                                       "upload a time series csv file"),
                                                                               fileType(fileType),
                                                                               fileName(fileName) { //constructor
    }

private:
    void uploadCSVFile() {
        this->dio->write("Please upload your local ");
        this->dio->write(fileType);
        this->dio->write(" CSV file.\n");
        //std::string fileName = getFileNameFromPath(this->dio->read()); //get the name of the file
        std::string clientData = "";
        std::string data = "";
        while (clientData.find("done") == std::string::npos) { // while not found done
            data = this->dio->read();
            clientData += data + "\n";
        }
        int doneIdx = clientData.find("done",0);
        clientData = clientData.substr(0, doneIdx); //cut the done word
        this->dio->uploadFile(clientData, fileName);
        this->dio->write("Upload complete.\n");
    }
};
class UploadTrainAndTestCommand : public Command {
    Command *uploadTrain;
    Command *uploadTest;
    SharedData* sharedData;
public:
    void execute() override {
        uploadTrain->execute();
        uploadTest->execute();
        this->sharedData->numOfStepsInTestFile  = getNumberOfStepsFromTestFile("anomalyTest.csv");
    }

    virtual ~UploadTrainAndTestCommand() {
        delete uploadTrain;
        delete uploadTest;
    }

    UploadTrainAndTestCommand(DefaultIO *IO, SharedData* sharedData) : Command(IO,
                                                       "upload a time series csv file") { //constructor
        this->sharedData = sharedData;
        this->uploadTrain = new UploadCommand(dio, "train", "anomalyTrain.csv");
        this->uploadTest = new UploadCommand(dio, "test", "anomalyTest.csv");
    }
private:
    int getNumberOfStepsFromTestFile(std::string fileName) {
        int numberOfLines = 0;
        std::string line;
        std::ifstream file(fileName);
        while (std::getline(file, line))
            ++numberOfLines;

        return numberOfLines - 1; //number of steps = number of lines - 1
    }

};

class ChangeCorrelationCommand : public Command {
private:
    SimpleAnomalyDetector *anomalyDetector;

public:
    ChangeCorrelationCommand(DefaultIO *IO, SharedData *sharedData) : Command(IO, "algorithm settings"),
                                                                      anomalyDetector(
                                                                              sharedData->anomalyDetector) { //constructor
    }

    void execute() override {
        float currentCorrelation = anomalyDetector->getCorrelationVal();
        this->dio->write("The current correlation threshold is ");
        this->dio->write(currentCorrelation);
        this->dio->write("\n");
        chooseThreshold();
    }

    virtual ~ChangeCorrelationCommand() {}

private:
    //let the client to change the correlation value
    void chooseThreshold() {
        this->dio->write("Type a new threshold");
        this->dio->write("\n");
        float newVal;
        this->dio->read(&newVal); //read the new correlation
        while (newVal < 0 || newVal > 1) { //if invalid input
            this->dio->write("please choose a value between 0 and 1");
            this->dio->write("\n");
            this->dio->read(&newVal);
        }
        this->anomalyDetector->setCorrelationValue(newVal); //update correlation
    }
};

//detect the anomaly in the file that uploaded
class DetectAnomalyCommand : public Command {
private:
    TimeSeriesAnomalyDetector *anomalyDetector;
    SharedData *sharedData;
public:
    DetectAnomalyCommand(DefaultIO *IO, SharedData *sharedData) : Command(IO, "detect anomalies"),
                                                                  anomalyDetector(
                                                                          sharedData->anomalyDetector) { //constructor
        this->sharedData = sharedData;
    }

    void execute() override {//learn and detects anomalies
        anomalyDetector->learnNormal("anomalyTrain.csv");
        sharedData->anomalyVec = anomalyDetector->detect("anomalyTest.csv");
        dio->write("anomaly detection complete.");
        dio->write("\n");
    }

    virtual ~DetectAnomalyCommand() {}

};

class DisplayAnomalyCommand : public Command {
private:
    SharedData *sharedData;
public:
    DisplayAnomalyCommand(DefaultIO *IO, SharedData *sharedData) : Command(IO, "display results") { //constructor
        this->sharedData = sharedData;
    }

    void execute() override {
        displayAnomalyResult();
        dio->write("Done.\n");
    }

    virtual ~DisplayAnomalyCommand() {}

private:
    //display all the anomalies to client
    void displayAnomalyResult() {
        for (auto &report : sharedData->anomalyVec) // for each anomaly
        {
            dio->write(report.timeStep);
            dio->write("\t");
            dio->write(report.description);
            dio->write("\n");
        }
    }
};
class AnalyzeAnomaliesCommand : public Command {
private:
    TimeSeriesAnomalyDetector *anomalyDetector;
    SharedData *sharedData;
public:
    AnalyzeAnomaliesCommand(DefaultIO *IO, SharedData *sharedData) : Command(IO,"upload anomalies and analyze results"),
                                                                anomalyDetector(sharedData->anomalyDetector) { //constructor
    this->sharedData = sharedData;
    }

    void execute() override {
        std::vector<std::pair<int, int>> pairVec = getAnomaliesFromClient(); //vector with the client anomaly
        std::vector<std::pair<int, int>> pairAnomalyVec = getTSAnomalies();
        //sort pairAnomalyVec
        sort(pairAnomalyVec.begin(), pairAnomalyVec.end(),
             [](const std::pair<int, int> &element1, const std::pair<int, int> &element2) {
                 return element1.second < element2.second;
             });
        sort(pairAnomalyVec.begin(), pairAnomalyVec.end(),
             [](const std::pair<int, int> &element1, const std::pair<int, int> &element2) {
                 return element1.first < element2.first;
             });
        sort(pairVec.begin(), pairVec.end(),
             [](const std::pair<int, int> &element1, const std::pair<int, int> &element2) {
                 return element1.second < element2.second;
             });
        sort(pairVec.begin(), pairVec.end(),
             [](const std::pair<int, int> &element1, const std::pair<int, int> &element2) {
                 return element1.first < element2.first;
             });
        int numbersOfSteps = this->sharedData->numOfStepsInTestFile;
        int numOfNormalSteps = getNumOfNormalSteps(pairVec, numbersOfSteps);
        int TP = 0; //true positive
        int FP = 0;
        //count the amount of element in pairVer that in range/ not in range
        for (auto &pair: pairVec)
            if (isPairInRange(pairAnomalyVec, pair))
                ++TP;
        for (auto &pair: pairAnomalyVec)
            if (!isPairInRange(pairVec, pair))
                ++FP;
        dio->write("True Positive Rate: ");
        float TPRate = std::floor((float) TP / pairVec.size() * 1000.) / 1000; //round the number
        dio->write(TPRate);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        float FPRate = std::floor((float) FP / numOfNormalSteps * 1000.) / 1000; //round the number
        dio->write(FPRate);
        dio->write("\n");

    }
    //return the amount of normal steps
    int getNumOfNormalSteps(std::vector<pair<int, int>> pairVec, int numberOfSteps) {
        int normalSteps = 0;
        set<int> anomalyStepsSet;
        //enter all the steps with anomaly to the set: "anomalyStepsSet"
        for (auto &anomalyPair: pairVec) { //for every pair (gap of time)
            for (int i = anomalyPair.first; i <= anomalyPair.second; i++) {
                anomalyStepsSet.insert(i);
            }
        }
        int numOfStepAnomalies = anomalyStepsSet.size();
        return numberOfSteps - numOfStepAnomalies; //return the valid steps
    }

    virtual ~AnalyzeAnomaliesCommand() {
    }

private:
    //return pairs of anomalies from my algorithm
    std::vector<std::pair<int, int>> getTSAnomalies() {
        std::set<std::string> anomalyFeaturesSet;
        auto anomalyReportVec = this->sharedData->anomalyVec; //get the real anomalies
        //enter all the features with anomaly
        for (auto &anomaly: anomalyReportVec) {
            anomalyFeaturesSet.insert(anomaly.description);
        }
        int numOfAnomalyFeature = anomalyFeaturesSet.size();
        map<string, vector<int>> featuresAnomaliesMap; //map that contain features and their anomalies
        for (auto &anomalyFeature: anomalyFeaturesSet) {
            featuresAnomaliesMap.insert({anomalyFeature, std::vector<int>()}); //map with feature and vector
        }
        for (auto &report: anomalyReportVec) { //for each report
            auto iterator = featuresAnomaliesMap.find(report.description);
            if (iterator == featuresAnomaliesMap.end())
                throw "error key not in the map"; //error
            else //if found in the map , push the anomaly to the vector
                iterator->second.push_back(report.timeStep);
        }
        //all the anomalies sort by features in featuresAnomaliesMap
        //sort all the anomalies by value in every vector
        for (auto it = featuresAnomaliesMap.begin(); it != featuresAnomaliesMap.end(); ++it) //for all the anomaly vec
        {
            sort(it->second.begin(), it->second.end());
        }

        std::vector<std::pair<int, int>> anomalyPairs;
        //unite all the pairs of sequence of anomalies
        //getAnomaliesPairsFromVec();
        for (auto it = featuresAnomaliesMap.begin(); it != featuresAnomaliesMap.end(); ++it) //for all the anomaly vec
        {
            auto newPairVector = getAnomaliesPairsFromVec(it->second); //for vector in the map
            anomalyPairs.insert(anomalyPairs.end(), newPairVector.begin(), newPairVector.end());
        }
        return anomalyPairs;
    }

    std::vector<std::pair<int, int>> getAnomaliesPairsFromVec(std::vector<int> anomalyVec) {
        std::vector<std::pair<int, int>> pairsVec;
        std::size_t size = anomalyVec.size();
        int first = anomalyVec[0]; //the first of pairs
        for (int i = 0; i < size - 1; ++i) {
            if (anomalyVec[i] + 1 == anomalyVec[i + 1]) { //if sequence of anomalies
                continue;
            } else { //if not sequence
                pairsVec.push_back(pair<int, int>(first, anomalyVec[i])); //enter the sequence
                first = anomalyVec[i + 1];
            }
        }
        pairsVec.push_back(pair<int, int>(first, anomalyVec[size - 1])); //enter the last pair
        return pairsVec;
    }

    //return pairs of anomalies (time steps)
    std::vector<std::pair<int, int>> getAnomaliesFromClient() {
        this->dio->write("Please upload your local anomalies file.\n");
        std::vector<std::pair<int, int>> anomalyPairs; //vector that contain the anomalies
        // std::string fileName = dio->read();
        std::string data = "";
        while (data.find("done") == std::string::npos) { // while not found done
            data = this->dio->read();
            if (data.find("done") == std::string::npos) //if not done
                anomalyPairs.push_back(getPairsFromString(data)); //add pairs to anomalyPairs
        }
        this->dio->write("Upload complete.\n");
        return anomalyPairs;
    }

    //return true if the element have common
    bool isPairInRange(std::vector<pair<int, int>> pairsVec, pair<int, int> element) {
        for (auto &pair: pairsVec) {
            //if in range
            if (pair.second >= element.first && pair.first <= element.first ||
                pair.second >= element.second && pair.first <= element.second ||
                pair.first >= element.first && pair.second <= element.second ||
                pair.first <= element.first && pair.second >= element.second) {
                return true;
            }
        }
        return false;
    }

    //extract 2 ints from string "int,int"
    std::pair<int, int> getPairsFromString(std::string pairsStr) {
        int valArray[] = {NOT_VAL, NOT_VAL}; // valArray = [-1,-1]
        get2IntFromStr(pairsStr, &valArray[0], &valArray[1]);
        return pair<int, int>(valArray[0], valArray[1]);
    }
    void get2IntFromStr(std::string pairsStr, int *num1, int *num2){
        int tempN1, tempN2;
        sscanf(pairsStr.c_str(), "%d,%d", num1, num2 );
    }
};
#endif /* COMMANDS_H_ */









