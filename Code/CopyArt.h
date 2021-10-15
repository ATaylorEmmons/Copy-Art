

struct Specimen {
  std::vector<Rectangle> traits;
  long score;
  Specimen() {}

  Specimen(int traitCount, RNG& rng, int maxX, int maxY, int minSize, int maxSize) {
    score = 0;
    traits.reserve(traitCount);

    for(int i = 0; i < traitCount; i++) {
      int x = rng.runifInt(0, maxX);
      int y = rng.runifInt(0, maxY);
      int s = rng.runifInt(minSize, maxSize);



      traits.push_back(Rectangle(x, y, s, s, Color::random(rng)));
    }
  }

  Specimen(Specimen* a, Specimen* b, int traitCount, RNG& rng,
      int colorRate = 5, int posXRate = 2, int posYRate = 1, int sizeRate = 2) {

    score = 0;
    traits.reserve(traitCount);

    for(int i = 0; i < traitCount; i++) {


      traits.push_back(Rectangle());

      int adjustRed = rng.runifFloat(-1, 1)*colorRate;//rng.rnormFloat(0, 1)*mutationRate;
      int adjustBlue = rng.runifFloat(-1, 1)*colorRate;//rng.rnormFloat(0, 1)*mutationRate;
      int adjustGreen = rng.runifFloat(-1, 1)*colorRate;//rng.rnormFloat(0, 1)*mutationRate;

      int adjustX = 0;//rng.runifFloat(-1, 1)*posXRate;
      int adjustY = 0;//rng.runifFloat(-1, 1)*posYRate;

      int adjustSize = 0;//rng.runifFloat(-1, 1)*sizeRate;

      int n = rng.rbinary();

      //Red
      n = rng.rbinary();
      int red = adjustRed + (n)*a->traits[i].color.r() + (1 - n)*b->traits[i].color.r();
      this->traits[i].color.setr(red);


      //Green
      n = rng.rbinary();
      int green = adjustGreen + (n)*a->traits[i].color.g() + (1 - n)*b->traits[i].color.g();
      this->traits[i].color.setg(green);

      //Blue
      n = rng.rbinary();
      int blue = adjustBlue + (n)*a->traits[i].color.b() + (1 - n)*b->traits[i].color.b();
      this->traits[i].color.setb(blue);

      //X
      n = rng.rbinary();
      this->traits[i].x = adjustX +  (n)*a->traits[i].x + (1 - n)*b->traits[i].x;

      //Y
      n = rng.rbinary();
      this->traits[i].y = adjustY + (n)*a->traits[i].y + (1 - n)*b->traits[i].y;

      //Width, Height
      n = rng.rbinary();
      this->traits[i].width = adjustSize + (n)*a->traits[i].width + (1 - n)*b->traits[i].width;
      this->traits[i].height = adjustSize + (n)*a->traits[i].height + (1 - n)*b->traits[i].height;


    }

  }

  bool operator<(const Specimen &comp) {
    return score < comp.score;
  }


};

long fitness(Image& image, Image& target) {

    double ret = 0;

    Color* imagePixels = image.getMemoryPtr();
    Color* targetPixels = target.getMemoryPtr();

    for(int i = 0; i < image.getHeight(); i++) {
        for(int j = 0; j < image.getWidth(); j++) {

            int index = i*image.getWidth() + j;

            uint8_t* imgChannels = imagePixels[index].channels;
            uint8_t* tgtChannels = targetPixels[index].channels;

            for(int channel = 0; channel < 3; channel++) {
              ret += abs(imgChannels[channel] - tgtChannels[channel]);
            }

        }
    }

    return ret;
}
