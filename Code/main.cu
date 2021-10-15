#include <iostream>
#include <vector>
#include <algorithm>

#include "utils.h"
#include "Rasterizer.h"
#include "CopyArt.h"



#define imin(a,b) (a<b?a:b)
const int N = 1920*1080*3;
const int THREADS = 1024;
const int BLOCKS = 1024;


__global__ void cuda_fitness(uint8_t* imageMemory, uint8_t* targetMemory, uint32_t* storage) {

    __shared__ int cache[THREADS];

    int t_id = threadIdx.x + blockIdx.x*blockDim.x;
    int stride = blockDim.x * gridDim.x;
    int cacheId = threadIdx.x;


    int store = 0;
    while(t_id < N) {
        store += abs(imageMemory[t_id] - targetMemory[t_id]);
        t_id += stride;
    }

    cache[cacheId] = store;

    __syncthreads();

    int i = blockDim.x/2;

    while( i != 0) {
      if(cacheId < i) {
        cache[cacheId] += cache[cacheId + i];
      }
      __syncthreads();
      i /= 2;
    }

    if(cacheId == 0) {
      storage[blockIdx.x] = cache[0];
    }
}

int main() {

  static const int WIDTH = 1920;
  static const int HEIGHT = 1080;


  RNG rng(Timer::now());

  Image buffer(WIDTH, HEIGHT);
  Image target("hk.png");


  bool useCuda = false;
  int genCount = 100;
  int specCount = 100;
  int rectCount = 5000;

  int minSize = 5;
  int maxSize = 20;

  int elitesToKeep = 4;
  float breedCutOff = .25;

  long startScore = 0;
  long bestScore = 0;

  Timer genTimer;
  long total_time = 0;

  Timer codeTimer;
  long crossover_time = 0;
  long render_time = 0;
  long fitness_time = 0;
  long sort_time = 0;

  std::vector<Specimen> specsA;
  specsA.reserve(specCount);

  std::vector<Specimen> specsB;
  specsB.reserve(specCount);

  std::vector<Specimen>* curSpec;
  std::vector<Specimen>* lastSpec;
  std::vector<Specimen>* tempSpec;

  Color clearColor = Color(0, 0, 0);

  int IMG_MEM_SIZE =  WIDTH*HEIGHT*3*sizeof(uint8_t);
  uint8_t* deviceTarget;
  uint8_t* deviceImage;
  uint32_t* deviceStorage;

  //uint8_t* resultStorage;

  uint32_t* resultStorage;

/*CUDA INIT */

  if(useCuda) {
    //target
    cudaMalloc( (void**)&deviceTarget, IMG_MEM_SIZE );
    cudaMemcpy( deviceTarget, (uint8_t*)target.getMemoryPtr(), IMG_MEM_SIZE, cudaMemcpyHostToDevice);

    resultStorage = (uint32_t *)malloc(IMG_MEM_SIZE);

    //image
    cudaMalloc( (void**)&deviceImage, IMG_MEM_SIZE );
    //result
    cudaMalloc( (void**)&deviceStorage, BLOCKS*sizeof(uint32_t));
  }


/* END CUDA INIT*/


  genTimer.start();
/* GENERATION 1 */
  for(int i = 0; i < specCount; i++) {

    specsA.push_back(Specimen(rectCount, rng, WIDTH, HEIGHT, minSize, maxSize));
    clear(clearColor, buffer);
    drawRects(specsA[i].traits.data(), rectCount, buffer);
    specsA[i].score = fitness(target, buffer);

    specsB.push_back(specsA[i]);

  }


  curSpec = &specsA;
  lastSpec = &specsB;

  codeTimer.start();
  std::sort(curSpec->begin(), curSpec->end());
  sort_time += codeTimer.stops();

  startScore = curSpec->at(0).score;

  total_time += genTimer.stops();
  println("Initilization: " + std::to_string(total_time/1000) + "ms");

/* THE REST */
  for(int curGen = 0; curGen < genCount; curGen++) {
    genTimer.start();

    for(int i = 0; i < elitesToKeep; i++) {
      curSpec->at(i) = lastSpec->at(i);
    }
    for(int i = elitesToKeep; i < specCount; i++) {

      //Crossover
      codeTimer.start();

      //Only the top half are parents
      int index1 = rng.runifInt(0, specCount*breedCutOff - 1);
      int index2 = rng.runifInt(0, specCount*breedCutOff - 1);

      while(index1 == index2) {
        index2 = rng.runifInt(0, specCount / 2 - 1);
      }

      Specimen* parentA = &lastSpec->at(index1);
      Specimen* parentB = &lastSpec->at(index2);

      curSpec->at(i) =  Specimen(parentA, parentB, rectCount, rng);
      crossover_time += codeTimer.stops();


      //Draw and Score
      codeTimer.start();
      clear(clearColor, buffer);
      drawRects(curSpec->at(i).traits.data(), rectCount, buffer);
      render_time += codeTimer.stops();

      codeTimer.start();

      if(useCuda) {
        //Cuda
        cudaMemcpy( deviceImage, (void*)buffer.getMemoryPtr(), IMG_MEM_SIZE, cudaMemcpyHostToDevice);
        cuda_fitness <<<BLOCKS, THREADS>>>(deviceImage, deviceTarget, deviceStorage);
        cudaMemcpy(resultStorage, deviceStorage, BLOCKS*sizeof(uint32_t), cudaMemcpyDeviceToHost);

        cudaDeviceSynchronize();
        uint32_t score = 0;
        for(int i = 0; i < BLOCKS; i++) {
          score += resultStorage[i];
        }

        curSpec->at(i).score = score;

      } else {
        curSpec->at(i).score = fitness(target, buffer);
      }
      fitness_time += codeTimer.stops();
    }



    codeTimer.start();
    std::sort(curSpec->begin(), curSpec->end());
    sort_time += codeTimer.stops();

    long genTime = genTimer.stops();
    total_time += genTime;

    std::string generationString =  " Score: " +  std::to_string(curSpec->at(0).score)
                                  + " Gen Count: " + std::to_string(curGen)
                                  + " Time: " + std::to_string(genTime/1000) +"ms";

    println(generationString);

    tempSpec = curSpec;
    curSpec = lastSpec;
    lastSpec = tempSpec;

  }

  bestScore = curSpec->at(0).score;
  double improvement = 1.0 - (double)bestScore/(double)startScore;
  drawRects(curSpec->at(0).traits.data(), rectCount, buffer);
  buffer.save("Copied.png");

  println("Improvement: " + std::to_string(improvement));
  println("Sort Time: " + std::to_string(sort_time/1000));
  println("Crossover Time: " + std::to_string(crossover_time/1000));
  println("Render Time: " + std::to_string(render_time/1000));
  println("Fitness Time: " + std::to_string(fitness_time/1000));
  println("Total Time: " + std::to_string(total_time/1000));


/* CLEANUP */
  if(useCuda) {
    cudaFree(deviceTarget);
    cudaFree(deviceImage);
    cudaFree(deviceStorage);

    free(resultStorage);
  }
}
















//
