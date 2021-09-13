#include <iostream>

#include "CopyArt.h"

int main()
{
    CopyArtParams params = {};

    params.populationSize = 32;
    params.generationCount = 10000;

    params.colorMutationRate = 5;
    params.positionMutationRate = 0;
    params.sizeMutationRate = 0;

    params.width = 128;
    params.height = 128;
    params.minSize = 3;
    params.maxSize = 5;
    params.rectCount = 2500;



    params.targetImageSrc = "oceanSunset.png";
    params.saveLocation = "OutImages/";

    CopyArt makeArt = CopyArt(params);

    makeArt.start();
    makeArt.saveTop(4);

    return 0;
}
