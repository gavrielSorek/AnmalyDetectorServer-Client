//
// Created by Gavriel on ID 318525185.
//
#include "CLI.h"
#include <stdlib.h>

CLI::CLI(DefaultIO *dio) {
    this->sharedData->anomalyDetector =  new HybridAnomalyDetector; //default detector
    this->dio = dio;
}

void CLI::start() {
    CLI::pushCommands(); //enter all the commands to the commands vector
    int chosenNum = -1; //the number that the client
    float readChosenNum;
    while (chosenNum != 6) { //interact with the client
        CLI::printMenu();//print the menu
        dio->read(&readChosenNum);
        chosenNum = (int) readChosenNum;
        if (chosenNum != 6) //if not exit
            this->commands[chosenNum - 1]->execute(); //execute the requested command
    }
}

void CLI::printMenu() {  //print the menu
    this->dio->write("Welcome to the Anomaly Detection Server.\n");
    this->dio->write("Please choose an option:\n");
    for (int i = 1; i <= this->commands.size(); i++) { //print all the commands options
        this->dio->write(std::to_string(i));
        this->dio->write("." + this->commands[i - 1]->getDescription());
        this->dio->write("\n");
    }
    this->dio->write("6.exit\n");
}

void CLI::pushCommands() {
    this->commands.push_back(new UploadTrainAndTestCommand(this->dio, this->sharedData));
    this->commands.push_back(new ChangeCorrelationCommand(this->dio, this->sharedData));
    this->commands.push_back(new DetectAnomalyCommand(this->dio, this->sharedData));
    this->commands.push_back(new DisplayAnomalyCommand(this->dio, this->sharedData));
    this->commands.push_back(new AnalyzeAnomaliesCommand(this->dio, this->sharedData));
}

CLI::~CLI() {
    size_t size = this->commands.size();
    for (int i = 0; i < size; ++i) {
        delete this->commands[i];
    }
    delete this->sharedData;
}

