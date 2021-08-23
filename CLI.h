//
// Created by Gavriel on ID 318525185.
//

#ifndef CLI_H_
#define CLI_H_

#include <string.h>
#include "commands.h"
#include "AnomalyDetector.h"
#include <vector>
using namespace std;

class CLI {
    DefaultIO* dio;
    std::vector<Command*> commands;
    SharedData* sharedData= new SharedData();
public:
    CLI(DefaultIO* dio);
    void start();
    virtual ~CLI();

    void printMenu();

    void pushCommands();
};

#endif /* CLI_H_ */

