
#include <vector>
#include <random>

#include "utils.h"

struct Specimen {

    double score;
    Image* ref_Image;
    std::vector<Rectangle> traits;

    static Specimen random(int32_t maxX, int32_t maxY, int32_t minSize, int32_t maxSize, uint32_t rectangleCount, Image* img) {

        Specimen ret;
        for(int32_t j = 0; j < rectangleCount; j++) {


            int32_t x = Globals::get().RNG.runifInt(0, maxX);
            int32_t y = Globals::get().RNG.runifInt(0, maxY);

            int32_t size = Globals::get().RNG.runifInt(minSize, maxSize);
            int32_t width =  size;
            int32_t height = size;
            Color color = Color::random();

            ret.traits.push_back(Rectangle(x, y, width, height, color));
        }

        ret.ref_Image = img;
        return ret;
    }


    Specimen(Specimen& a, Specimen& b, Image* img,
              int32_t colorMutationRate, int32_t positionMutationRate, int32_t sizeMutationRate) {

        this->ref_Image = img;


        for(int32_t i = 0; i < a.traits.size(); i++) {
            this->traits.push_back(Rectangle());


            int32_t adjustRed = Globals::get().RNG.rnormFloat(0, 1)*colorMutationRate;
            int32_t adjustBlue = Globals::get().RNG.rnormFloat(0, 1)*colorMutationRate;
            int32_t adjustGreen = Globals::get().RNG.rnormFloat(0, 1)*colorMutationRate;


            int32_t adjustX = Globals::get().RNG.rnormFloat(0, 1)*positionMutationRate;
            int32_t adjustY = Globals::get().RNG.rnormFloat(0, 1)*positionMutationRate;

            int32_t adjustWidth = Globals::get().RNG.rnormFloat(0, 1)*sizeMutationRate;
            int32_t adjustHeight = Globals::get().RNG.rnormFloat(0, 1)*sizeMutationRate;


            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].x = a.traits[i].x + adjustX;
            } else {
                this->traits[i].x = b.traits[i].x + adjustX;
            }

            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].y = a.traits[i].y + adjustY;
            } else {
                this->traits[i].y = b.traits[i].y + adjustY;
            }

            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].width = a.traits[i].width + adjustWidth;
                this->traits[i].height = a.traits[i].height + adjustHeight;
            } else {
                this->traits[i].width = b.traits[i].width + adjustWidth;
                this->traits[i].height = b.traits[i].height + adjustHeight;
            }

            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].color.setr(a.traits[i].color.r() + adjustRed);
            } else {
                this->traits[i].color.setr(b.traits[i].color.r() + adjustRed);
            }

            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].color.setg(a.traits[i].color.g() + adjustGreen);
            } else {
                this->traits[i].color.setg(b.traits[i].color.g() + adjustGreen);

            }

            if(Globals::get().RNG.runifFloat(0, 1) < .5) {
                this->traits[i].color.setb(a.traits[i].color.b() + adjustBlue);
            } else {
                this->traits[i].color.setb(b.traits[i].color.b() + adjustBlue);

            }
        }
      }


    bool operator<(const Specimen &comp) {
        return score < comp.score;
    }

  private:
        Specimen() {}


};
