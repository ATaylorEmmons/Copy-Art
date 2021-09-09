#include <vector>
#include <algorithm>
#include <random>


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

struct CopyArt {

        int32_t populationSize;
        int32_t generationCount;
        float mutationChance;

        int32_t width;
        int32_t height;
        int32_t rectangleCount;

        Image* target;
        std::string saveLocation;

        Rasterizer canvas;
        std::vector<Image*> images;

        std::default_random_engine gen;
        std::uniform_int_distribution<int> randomX;
        std::uniform_int_distribution<int> randomY;
        std::uniform_int_distribution<int> randomSize;
        std::uniform_real_distribution<float> runif;

        std::vector<Specimen> specimens;

        CopyArt(int32_t popSize, int32_t genCount, float mutChance,
                int32_t width, int32_t height, int32_t minSize, int32_t maxSize, int32_t rectCount,
                std::string targetSrc, std::string saveLocation) {

            populationSize = popSize;
            generationCount = genCount;
            mutationChance = mutChance;
            this->width = width;
            this->height = height;
            rectangleCount = rectCount;
            target = new Image(targetSrc);
            this->saveLocation = saveLocation;

            randomX = std::uniform_int_distribution<int> (0, width);
            randomY = std::uniform_int_distribution<int> (0, height);
            randomSize = std::uniform_int_distribution<int> (minSize, maxSize);
            runif = std::uniform_real_distribution<float>(0, 1);

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
                    int32_t len = randomSize(gen);
                    Color color = Color::random();
                    specimens.at(i).traits.push_back(Rectangle(x, y, len, len, color));
                }

                canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
            }

        }

        void start() {

            for(int32_t i = 0; i < generationCount; i++) {
                double score = nextGeneration();

                std::cout << score << std::endl;
            }
        }

        //Hard Coded to 8 for now
        double nextGeneration() {

            double bestScore = -1;

            for(int32_t i = 0; i < populationSize; i++) {

                specimens.at(i).score = fitness(specimens.at(i).ref_Image, target);
            }

            std::sort(specimens.begin(), specimens.end());

            bestScore = specimens.front().score;

            specimens.at(4) = child(specimens.at(0), specimens.at(1), specimens.at(4).ref_Image);
            specimens.at(5) = child(specimens.at(0), specimens.at(2), specimens.at(5).ref_Image);
            specimens.at(6) = child(specimens.at(1), specimens.at(3), specimens.at(6).ref_Image);
            specimens.at(7) = child(specimens.at(2), specimens.at(3), specimens.at(7).ref_Image);

            for(int32_t i = 4; i < populationSize; i++) {
                canvas.setBuffer(specimens.at(i).ref_Image);
                canvas.clear({10, 10, 10});
                canvas.drawRects(specimens.at(i).traits.data(), rectangleCount);
            }

            return bestScore;
        }


        Specimen child(Specimen& a, Specimen& b, Image* img) {

            Specimen child(img);

            for(int32_t i = 0; i < rectangleCount; i++) {
                if(runif(gen) < mutationChance) {

                    child.traits.push_back(Rectangle(randomX(gen), randomY(gen), randomSize(gen), randomSize(gen), Color::random()));

                } else {
                    Rectangle trait;

                    if(runif(gen) < .5) {
                        trait.x = a.traits[i].x;
                    } else {
                        trait.x = b.traits[i].x;
                    }

                    if(runif(gen) < .5) {
                        trait.y = a.traits[i].y;
                    } else {
                        trait.y = b.traits[i].y;
                    }

                    if(runif(gen) < .5) {
                        trait.width = a.traits[i].width;
                        trait.height = a.traits[i].height;
                    } else {
                        trait.width = b.traits[i].width;
                        trait.height = b.traits[i].height;
                    }

                    if(runif(gen) < .5) {
                        trait.color.setr(a.traits[i].color.r());
                    } else {
                        trait.color.setr(b.traits[i].color.r());

                    }

                    if(runif(gen) < .5) {
                        trait.color.setg(a.traits[i].color.g());
                    } else {
                        trait.color.setg(b.traits[i].color.g());

                    }

                    if(runif(gen) < .5) {
                        trait.color.setb(a.traits[i].color.b());
                    } else {
                        trait.color.setb(b.traits[i].color.b());

                    }


                    child.traits.push_back(trait);

                }
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

         void saveImages(int32_t generation) {
            for(int32_t i = 0; i < populationSize; i++) {

                std::string loc = saveLocation +  std::to_string(generation) + "_"  + std::to_string(i)  + ".png";

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

