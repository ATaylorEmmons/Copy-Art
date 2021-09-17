
#include <chrono>

#ifndef __utils_h_
#define __utils_h_


struct _RNG {

   std::default_random_engine generator;

   _RNG() {}

   //Includes min and max
   int32_t runifInt(int32_t min, int32_t max) {

        std::uniform_int_distribution<int32_t> randomNumber(min, max);

        return randomNumber(generator);

    }

    //Does not include max
    float runifFloat(float min, float max) {

        std::uniform_real_distribution<float> randomNumber(min, max);

        return randomNumber(generator);
    }


    float rnormFloat(float mean, float sd) {

        std::normal_distribution<float> randomNumber(mean, sd);

        return randomNumber(generator);
    }

};

struct Timer {

    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;

    Timer() {}

    void start() {
        t0 = std::chrono::high_resolution_clock::now();
    }

    long stop() {
        t1 =  std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    	return duration.count();
    }
};


class Globals {
    public:
        static Globals& get() {

            static Globals instance;

            return instance;
        }

        _RNG RNG;

        ~Globals() {}


    private:
        Globals(Globals const&){}
        void operator=(Globals const&){}

        Globals() {}

};

#endif
