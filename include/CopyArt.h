#include <vector>
#include <algorithm>
#include <random>

#include "utils.h"
#include "Rasterizer.h"
#include "Specimen.h"


struct CopyArtParams {
    int32_t populationSize;
    int32_t generationCount;

    float eliteRatio;
    float childRatio;
    float randomsRatio;

    float mutationChance;
    uint8_t colorMutationRate;
    int32_t positionMutationRate;
    int32_t sizeMutationRate;

    int32_t width;
    int32_t height;
    int32_t minSize;
    int32_t maxSize;

    int32_t rectCount;

    std::string targetImageSrc;
    std::string saveLocation;

};


struct CopyArt {

        int32_t populationSize;
        int32_t generationCount;

        float eliteRatio;
        float childRatio;
        float randomsRatio;

        float mutationChance;
        uint8_t colorMutationRate;

        int32_t positionMutationRate;
        int32_t sizeMutationRate;

        int32_t minSize;
        int32_t maxSize;
        int32_t width;
        int32_t height;
        int32_t rectangleCount;

        Image* target;
        std::string saveLocation;

        Rasterizer canvas;
        std::vector<Image*> images;

        std::vector<Specimen> specimens;

/* Constructor */
        CopyArt(CopyArtParams& params) {

            populationSize = params.populationSize;
            generationCount = params.generationCount;

            eliteRatio = params.eliteRatio;
            childRatio = params.childRatio;
            randomsRatio = params.randomsRatio;

            mutationChance = params.mutationChance;
            colorMutationRate = params.colorMutationRate;
            positionMutationRate = params.positionMutationRate;
            sizeMutationRate = params.sizeMutationRate;

            width = params.width;
            height = params.height;
            minSize = params.minSize;
            maxSize = params.maxSize;
            rectangleCount = params.rectCount;

            target = new Image(params.targetImageSrc);
            saveLocation = params.saveLocation;

            for(int32_t i = 0; i < populationSize; i++) {
                images.push_back(new Image(width, height));

                //Create entirely random population to start
                specimens.push_back(Specimen::random(width, height, minSize, maxSize, rectangleCount, images.at(i)));

                canvas.setBuffer(images.at(i));
                canvas.clear({10, 10, 10});
                canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
            }

        }

/*Starts the search and is the main loop*/
        void start(bool videoMode, bool botMode) {

            double startScore = scoreGeneration();
            double currentScore = startScore;

            Timer timer;
            for(int32_t i = 0; i < generationCount; i++) {

                timer.start();
                createGeneration();
                currentScore = scoreGeneration();


                if(!botMode) {
                  std::string printStr = std::to_string(i) + ", " + std::to_string(currentScore) + ", " + std::to_string(timer.stop());
                  std::cout << printStr << std::endl;
                }

                if(videoMode) {
                  std::string name = saveLocation + "/" + "Frame" + std::to_string(i) + ".png";
                  specimens.at(0).ref_Image->save(name);
                }

            }

            std::cout << 1.0 -  double(currentScore)/double(startScore) << ", " << (float)timer.stop()/1000.0;

            if(!botMode)
              std::cout << std::endl;
        }


/*Creates the next generation

*/
        void createGeneration() {

            int32_t endEliteRange = specimens.size()*eliteRatio;
            int32_t endChildrenRange = specimens.size()*(eliteRatio + childRatio);
            int32_t endRandomRange = specimens.size();

            /*The elite portion of the generation is never removed*/

            //Create the child portion of the generation
            for(int32_t i = endEliteRange; i < endChildrenRange; i++) {

                int32_t parent1 = Globals::get().RNG.runifInt(0, endEliteRange - 1);
                int32_t parent2 = Globals::get().RNG.runifInt(0, endEliteRange - 1);

                //Avoid parents being the same
                if(endEliteRange > 1) {
                  while(parent1 == parent2) {
                      parent2 = Globals::get().RNG.runifInt(0, endEliteRange - 1);
                  }
                }

                specimens.at(i) = Specimen(specimens.at(parent1), specimens.at(parent2), specimens.at(i).ref_Image,
                                            colorMutationRate, positionMutationRate, sizeMutationRate);

            }


          /*Elite X Randoms
            Have one of the parents be random
          */

          for(int32_t i = endChildrenRange; i < endRandomRange; i++) {
              int32_t parent1 = Globals::get().RNG.runifInt(0, endEliteRange - 1);

              Specimen randParent2 = Specimen::random(width, height, minSize, maxSize, rectangleCount, NULL);

              specimens.at(i) = Specimen(specimens.at(parent1), randParent2, specimens.at(i).ref_Image, 0, 0, 0);
                                          //colorMutationRate, positionMutationRate, sizeMutationRate);
          }

            //Draw each specimen
          for(int32_t i = endEliteRange; i < populationSize; i++) {
              canvas.setBuffer(specimens.at(i).ref_Image);
              canvas.clear({10, 10, 10});
              canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
          }

      }


/*Score each generation
    Optimization:
        The elites are always scored except for the
        first time they are created.
        elites = -1 means don't run the fitness function
        on them.

*/
        double scoreGeneration(float elites = -1) {
            int32_t endEliteRange = 0;

            if(elites != -1) {
              endEliteRange = specimens.size()*elites;
            }

            for(int32_t i = endEliteRange; i < populationSize; i++) {
                specimens.at(i).score = fitness(specimens.at(i).ref_Image, target);
            }

            //Sort specimen by score in ascending order
            // Lower is a better score
            std::sort(specimens.begin(), specimens.end());

            //Return the best score
            return specimens.front().score;
        }

/* Saves the top imgCount images in the folder named saveLocation
    -Make sure the folder exists or nothing is written it seems.
 */
        void saveImages(int32_t imgCount) {
            for(int32_t i = 0; i < imgCount; i++) {

                std::string loc = saveLocation + "/" + std::to_string(i) + ".png";

                specimens.at(i).ref_Image->save(loc);

            }
        }

/*Destructor
  deletes the image objects*/
        ~CopyArt() {
            for(int32_t i = 0; i < populationSize; i++) {
                delete images.at(i);
            }

            delete target;
        }


/* Compares a given image and the target images

    Subtracts the targets pixels from the given image(vector subtraction)
      then sums the square of the resulting vectors components and adds that
      to ret.

    Lower is a better score.
*/
        double fitness(Image* image, Image* target) {

            double ret = 0;

            Color* imagePixels = image->getMemoryPtr();
            Color* targetPixels = target->getMemoryPtr();

            for(int32_t i = 0; i < image->getHeight(); i++) {
                for(int32_t j = 0; j < image->getWidth(); j++) {

                    int32_t index = i*image->getWidth() + j;

                    ret += (imagePixels[index] - targetPixels[index]).squareSum();

                }
            }

            return ret;
      }

};
