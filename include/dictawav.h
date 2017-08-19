/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 13/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_DICTAWAV_H
#define DICTAWAV_DICTAWAV_H

#include <iostream>
#include <fstream>
#include <string>
#include "wav_handler/WavHandler.h"
#include "preprocessor/PreProcessor.h"
#include "classificator/KernelCanvas.hpp"
#include "classificator/Wisard.h"

namespace DictaWav
{
  class DictaWav
  {
    private:
      std::unique_ptr<WavHandler> wavHandlerPtr;
      std::unique_ptr<PreProcessor> preProcessorPtr;
      KernelCanvas<Frame<double>> kernelCanvas;
      Wisard wisard;
    
    public:
      DictaWav() :
          kernelCanvas(1024, 13),
          wisard(1024, 3)
      { }
      
      void newTraining(std::string wavTrainingFile, std::string className);
      void classify(std::string wavFileToClassify);
    
    private:
      void readAndProcessWavFile(std::string wavFile);
  };
}

#endif //DICTAWAV_DICTAWAV_H
