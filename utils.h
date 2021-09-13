
#include <chrono>

struct Timer {

    std::chrono::high_resolution_clock::time_point t0;
    std::chrono::high_resolution_clock::time_point t1;

    void start() {
        t0 = std::chrono::high_resolution_clock::now();
    }

    long stop() {
        t1 =  std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    	return duration.count();
    }


};




