#include <cmath>
#include <iomanip>
#include <array>
#include <vector>
#include <algorithm>
#include <iterator>
#include <thread>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include "nvidia_info.h"

using namespace std;

int gpuLoad, gpuTemp, cpuTemp;
FILE *amdGpuFile = nullptr, *amdTempFile = nullptr, *cpuTempFile = nullptr;


int numCpuCores = std::thread::hardware_concurrency();
pthread_t cpuThread, gpuThread, cpuInfoThread, nvidiaSmiThread;

string exec(string command) {
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}


void *cpuInfo(void *){
    rewind(cpuTempFile);
    fflush(cpuTempFile);
    if (fscanf(cpuTempFile, "%d", &cpuTemp) != 1)
        cpuTemp = 0;
    cpuTemp /= 1000;
    pthread_detach(cpuInfoThread);

    return NULL;
}

void *getNvidiaGpuInfo(void *){
    if (!nvmlSuccess)
        checkNvidia();

    if (nvmlSuccess){
        getNvidiaInfo();
        gpuLoad = nvidiaUtilization.gpu;
        gpuTemp = nvidiaTemp;
    }

    pthread_detach(nvidiaSmiThread);
    return NULL;
}

void *getAmdGpuUsage(void *){
    rewind(amdGpuFile);
    fflush(amdGpuFile);
    if (fscanf(amdGpuFile, "%d", &gpuLoad) != 1)
        gpuLoad = 0;

    rewind(amdTempFile);
    fflush(amdTempFile);
    if (fscanf(amdTempFile, "%d", &gpuTemp) != 1)
        gpuTemp = 0;
    gpuTemp /= 1000;

    pthread_detach(gpuThread);
    return NULL;
}
