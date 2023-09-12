#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


std::vector<std::string> readGraphData(std::string fileName){
    std::vector<std::string> strVec;
    std::ifstream inputFile(fileName);

    if(inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            strVec.push_back(line);
            std::cout << line << std::endl;
        }
        inputFile.close();
    }else{
        std::cout<<"Failed to open the file." <<std::endl;
    }
    return strVec;
}

std::vector<std::string> splitString(std::string& str, char delimiter){
    std::vector<std::string> tokens;
    std::istringstream  iss(str);
    std::string token;

    while(std::getline(iss, token, delimiter)){
        tokens.push_back(token);
    }
    return tokens;
}

