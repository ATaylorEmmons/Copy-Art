#include <iostream>

#include "CopyArt.h"




int main()
{
    CopyArtParams params = {};

    params.populationSize = 128;
    params.generationCount = 1000;

    params.eliteRatio = .40;
    params.childRatio = .40;
    params.randomsRatio = .20;

    params.colorMutationRate = 5;
    params.positionMutationRate = 0;
    params.sizeMutationRate = 0;

    params.width = 128;
    params.height = 128;
    params.minSize = 5;
    params.maxSize = 8;
    params.rectCount = 1000;



    params.targetImageSrc = "oceanSunset.png";
    params.saveLocation = "OutImages/";

    CopyArt makeArt = CopyArt(params);


    makeArt.start();
    makeArt.saveImages(4);

    return 0;
}
