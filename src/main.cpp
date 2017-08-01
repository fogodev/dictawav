#include <iostream>
#include <sstream>
#include "../include/dictawav.h"

int main(int argc, char** argv)
{
  DictaWav::DictaWav dictaWav;
  
  std::ifstream trainingWavsFile;
  std::string wavFileAndClassName;
  std::string wavFile;
  std::string className;
  
  if (argc > 1) {
    trainingWavsFile.open(argv[1]);
    
    if (trainingWavsFile.is_open()) {
      while (std::getline(trainingWavsFile, wavFileAndClassName)) {
        std::stringstream wavFileAndClassNameStream(wavFileAndClassName);
        wavFileAndClassNameStream >> wavFile >> className;
        dictaWav.newTraining(wavFile, className);
      }
    }
  }
  
  bool flag = true;
  std::string mode;
  do {
    std::cout << "Enter \"t\" to train, \"c\" to classify or \"q\" to quit:" << std::endl;
    std::cin >> mode;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    if (mode == "t") {
      std::cout << "Enter wav file name and respective class name to train:" << std::endl;
      std::cin >> wavFile >> className;
      dictaWav.newTraining(wavFile, className);
      std::cout << "Training complete!" << std::endl;
    }
    else if (mode == "c") {
      std::cout << "Enter wav file name to classify:" << std::endl;
      std::cin >> wavFile;
      dictaWav.classify(wavFile);
    }
    else if (mode == "q")
      flag = false;
    else
      std::cout << "Not a valid command" << std::endl;
    
  }
  while (flag);
  
  return 0;
}