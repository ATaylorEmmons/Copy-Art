#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include "CopyArt.h"

CopyArtParams parseParams(std::string path) {

    std::map<std::string, int> fileParameters;
    CopyArtParams params = {0};
    std::ifstream readStr(path);

    std::string delimeter = "=";

    if(!readStr) {
        throw std::invalid_argument("Something was wring the the input data file/path");
    }

    for(std::string line; std::getline(readStr, line);) {
        if(line[0] == '#') {
            continue;
        }

        if(line.empty()) {
            continue;
        }
        std::string token = line.substr(0, line.find(delimeter));
        std::string value = line.substr(line.find(delimeter) + 1, line.size());

        fileParameters.insert(std::pair<std::string, int32_t>(token, std::stoi(value)));

    }

    params.populationSize = fileParameters["populationSize"];
    params.generationCount =  fileParameters["generationCount"];

    params.eliteRatio =  fileParameters["eliteRatio"] / 100.0f;
    params.childRatio =  fileParameters["childRatio"] / 100.0f;
    params.randomsRatio =  fileParameters["randomsRatio"] / 100.0f;

    params.colorMutationRate = fileParameters["colorMutationRate"];
    params.positionMutationRate = fileParameters["positionMutationRate"];
    params.sizeMutationRate =  fileParameters["sizeMutationRate"];

    params.width =  fileParameters["width"];
    params.height =  fileParameters["height"];
    params.minSize =  fileParameters["minSize"];
    params.maxSize =  fileParameters["maxSize"];
    params.rectCount =  fileParameters["rectCount"];


    return params;
}

int main(int argc, char **argv)
{
    if(argc < 4) {
        std::cout << "TargetImage, OutPutDirectory, ParamsFileLocation" << std::endl;
        std::cout << "Optional: videoMode botMode" << std::endl;
        std::cout << "videoMode: Have the best image of each generation saved in order to make a video" << std::endl;
        std::cout << "botMode: Have the output be only analytics to read from python " << std::endl;
        return -1;
    }

    CopyArtParams params = parseParams(std::string(argv[3]));
    params.targetImageSrc = std::string(argv[1]);
    params.saveLocation = std::string(argv[2]);


    std::string optionalInputs = "";
    //4 is the number of non optional inputs
    for(int32_t i = 4; i < argc; i++) {
        optionalInputs += argv[i];
    }

    //    Have the best image of each generation saved in order to make a video
    bool videoMode = (optionalInputs.find("videoMode") != -1);
    //    Have the output be only analytics to automate running
    bool botMode = (optionalInputs.find("botMode") != -1);

    CopyArt makeArt = CopyArt(params);
    makeArt.start(videoMode, botMode);
    makeArt.saveImages(4);

    return 0;
}
