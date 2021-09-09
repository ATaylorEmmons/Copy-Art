#include <iostream>

#include "CopyArt.h"


int main()
{   //hard coded to 8 internally
    int32_t populationSize = 8;
    int32_t generationCount = 100000;
    float mutationChance = .35;

    int32_t imageWidth = 128;
    int32_t imageHeight = 128;
    int32_t minSize = 1;
    int32_t maxSize = 2;
    int32_t rectCount = 10000;

    std::string targetImageSrc = "oceanSunset.png";
    std::string saveLocation = "OutImages/";

    CopyArt makeArt = CopyArt(populationSize, generationCount, mutationChance,
                              imageWidth, imageHeight, minSize, maxSize, rectCount,
                              targetImageSrc, saveLocation);

    makeArt.start();
    makeArt.saveImages();

    return 0;
}
