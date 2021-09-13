#include <vector>
#include <algorithm>
#include <random>

#include "utils.h"
#include "Rasterizer.h"

struct Specimen {

    double score;
    Image* ref_Image;
    std::vector<Rectangle> traits;

    Specimen(Image* image) {
        ref_Image = image;
    }

    bool operator<(const Specimen &comp) {

        return score < comp.score;

    }

};

struct CopyArtParams {
    int32_t populationSize;
    int32_t generationCount;

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

        float mutationChance;
        uint8_t colorMutationRate;

        int32_t positionMutationRate;
        int32_t sizeMutationRate;

        int32_t width;
        int32_t height;
        int32_t rectangleCount;

        Image* target;
        std::string saveLocation;

        Rasterizer canvas;
        std::vector<Image*> images;

        std::default_random_engine gen;
        std::uniform_int_distribution<int> randomParent;
        std::uniform_int_distribution<int> randomX;
        std::uniform_int_distribution<int> randomY;
        std::uniform_int_distribution<int> randomSize;

        std::normal_distribution<float> stdNormal;
        std::uniform_real_distribution<float> runif;



        std::vector<Specimen> specimens;

        CopyArt(CopyArtParams& params) {

            populationSize = params.populationSize;
            generationCount = params.generationCount;

            mutationChance = params.mutationChance;
            colorMutationRate = params.colorMutationRate;
            positionMutationRate = params.positionMutationRate;
            sizeMutationRate = params.sizeMutationRate;

            width = params.width;
            height = params.height;
            rectangleCount = params.rectCount;

            target = new Image(params.targetImageSrc);
            saveLocation = params.saveLocation;

            randomParent = std::uniform_int_distribution<int>(0, populationSize - 1);

            randomX = std::uniform_int_distribution<int> (0, width);
            randomY = std::uniform_int_distribution<int> (0, height);

            randomSize = std::uniform_int_distribution<int> (params.minSize, params.maxSize);

            runif = std::uniform_real_distribution<float>(0, 1);

             stdNormal = std::normal_distribution<float>(0, 1);

            for(int32_t i = 0; i < populationSize; i++) {

                images.push_back(new Image(width, height));
                canvas.setBuffer(images.at(i));
                canvas.clear({10, 10, 10});
            }


            /* Make the first generation */

            for(int32_t i = 0; i < populationSize; i++) {

                specimens.push_back(Specimen(images.at(i)));
                canvas.setBuffer(specimens.at(i).ref_Image);
                //Generate random rectangles
                for(int32_t j = 0; j < rectangleCount; j++) {

                    int32_t x = randomX(gen);
                    int32_t y = randomY(gen);
                    int32_t width = randomSize(gen);
                    int32_t height = randomSize(gen);
                    Color color = Color::random();

                    specimens.at(i).traits.push_back(Rectangle(x, y, width, height, color));
                }

                canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
            }

        }

        void start() {

            double startScore = nextGeneration();
            double currentScore = startScore;

            Timer timer;
            for(int32_t i = 0; i < generationCount - 1; i++) {
                timer.start();
                currentScore = nextGeneration();

                std::string printStr = std::to_string(i) + ", " + std::to_string(currentScore) + ", " + std::to_string(timer.stop());
                std::cout << printStr << std::endl;

            }

            std::cout << "Fit: " << 1.0 -  double(currentScore)/double(startScore) << std::endl;
        }

        double nextGeneration() {

            for(int32_t i = 0; i < populationSize; i++) {

                specimens.at(i).score = fitness(specimens.at(i).ref_Image, target);
            }

            std::sort(specimens.begin(), specimens.end());

            double bestScore = specimens.front().score;

            int32_t half = specimens.size() / 2;

            for(int32_t i = half; i < specimens.size(); i++) {

                int32_t parent1 = randomParent(gen);
                int32_t parent2 = randomParent(gen);

                while(parent1 == parent2) {
                    parent2 = randomParent(gen);
                }

                specimens.at(i) = child(specimens.at(parent1), specimens.at(parent2), specimens.at(i).ref_Image);

            }

            for(int32_t i = half; i < populationSize; i++) {
                canvas.setBuffer(specimens.at(i).ref_Image);
                canvas.clear({10, 10, 10});
                canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
            }



            return bestScore;
        }


        Specimen child(Specimen& a, Specimen& b, Image* img) {

            Specimen child(img);

            for(int32_t i = 0; i < rectangleCount; i++) {
                Rectangle trait;

                int32_t adjustRed = 0;
                int32_t adjustBlue = 0;
                int32_t adjustGreen = 0;

                int32_t adjustX = 0;
                int32_t adjustY = 0;

                int32_t adjustWidth = 0;
                int32_t adjustHeight = 0;


                adjustRed = stdNormal(gen)*colorMutationRate;
                adjustBlue = stdNormal(gen)*colorMutationRate;
                adjustGreen = stdNormal(gen)*colorMutationRate;


                adjustX = stdNormal(gen)*positionMutationRate;
                adjustY = stdNormal(gen)*positionMutationRate;

                adjustWidth = stdNormal(gen)*sizeMutationRate;
                adjustHeight = stdNormal(gen)*sizeMutationRate;




                if(runif(gen) < .5) {
                    trait.x = a.traits[i].x + adjustX;
                } else {
                    trait.x = b.traits[i].x + adjustX;
                }

                if(runif(gen) < .5) {
                    trait.y = a.traits[i].y + adjustY;
                } else {
                    trait.y = b.traits[i].y + adjustY;
                }

                if(runif(gen) < .5) {
                    trait.width = a.traits[i].width + adjustWidth;
                    trait.height = a.traits[i].height + adjustHeight;
                } else {
                    trait.width = b.traits[i].width + adjustWidth;
                    trait.height = b.traits[i].height + adjustHeight;
                }

                if(runif(gen) < .5) {
                    trait.color.setr(a.traits[i].color.r() + adjustRed);
                } else {
                    trait.color.setr(b.traits[i].color.r() + adjustRed);

                }

                if(runif(gen) < .5) {
                    trait.color.setg(a.traits[i].color.g() + adjustGreen);
                } else {
                    trait.color.setg(b.traits[i].color.g() + adjustGreen);

                }

                if(runif(gen) < .5) {
                    trait.color.setb(a.traits[i].color.b() + adjustBlue);
                } else {
                    trait.color.setb(b.traits[i].color.b() + adjustBlue);

                }

                child.traits.push_back(trait);

            }


            child.ref_Image = img;
            return child;
        }

        void saveImages() {
            for(int32_t i = 0; i < populationSize; i++) {

                std::string loc = saveLocation + std::to_string(i) + ".png";

                specimens.at(i).ref_Image->save(loc);

            }
        }
        void saveTop(int32_t imgCount) {
            for(int32_t i = 0; i < imgCount; i++) {

                std::string loc = saveLocation + std::to_string(i) + ".png";

                specimens.at(i).ref_Image->save(loc);

            }
        }

        ~CopyArt() {
            for(int32_t i = 0; i < populationSize; i++) {

                delete images.at(i);
            }

            delete target;

        }

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

