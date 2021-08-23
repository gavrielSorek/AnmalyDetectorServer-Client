
//
// Created by gavri on 16/12/2020.
//
#ifndef STANDARD_IO_H
#define STANDARD_IO_H
#include "commands.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

class StandardIO: public DefaultIO {
public:
    StandardIO() {

    }
    //write to file the string: fileString.
    void uploadFile(std::string fileString, std::string fileName) override {
        std::ofstream file;
        file.open (fileName);
        file << fileString;
        file.close();
    }
    std::string downloadFile(std::string fileName) override {/////////////////////
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
    void write(std::string txt) override {
        std::cout << txt;
    }
    std::string read() override {
        std::string str;
        std::cin >> str;
        return str;
    }
    //write float number
    void write(float f) {
        std::cout << f;
    }
    //read float number
    void read(float* f) {
        std::cin>>(*f);
    }
};


#endif //CLI_CPP_STANDARDIO_H




