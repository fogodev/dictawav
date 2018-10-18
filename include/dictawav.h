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
#include "classificator/KernelCanvas.h"
#include "classificator/Wisard.h"

namespace DictaWav {

class DictaWav {
 private:
  KernelCanvas kernelCanvas;
  Wisard wisard;

 public:
  DictaWav(
      size_t kernelCanvasNumKernels,
      size_t kernelCanvasKernelDimension,
      int kernelCanvasOutputFactor,
      size_t wisardRetinaSize,
      size_t wisardNumBitsAddr,
      bool wisardUseBleaching = true,
      double wisardConfidenceMinimumRate = 0.1,
      unsigned wisardBleachingThreshold = 1,
      bool wisardRandomizePositions = true,
      bool wisardIsCumulative = true
  ) :
      kernelCanvas(
          kernelCanvasNumKernels,
          kernelCanvasKernelDimension,
          kernelCanvasOutputFactor
      ),
      wisard(
          wisardRetinaSize,
          wisardNumBitsAddr,
          wisardUseBleaching,
          wisardConfidenceMinimumRate,
          wisardBleachingThreshold,
          wisardRandomizePositions,
          wisardIsCumulative
      ) {}

  void train(std::string wavTrainingFile, std::string className) {
    this->wisard.train(this->readAndProcessWavFile(wavTrainingFile), className);
  }

  void forget(std::string wavTrainingFile, std::string className) {
    this->wisard.forget(this->readAndProcessWavFile(wavTrainingFile), className);
  }

  std::string classify(std::string wavFileToClassify) {
    return this->wisard.classify(this->readAndProcessWavFile(wavFileToClassify));
  }

  std::pair<std::string, double> classificationAndProbability(std::string wavFileToClassify) {
    return this->wisard.classificationAndProbability(
        this->readAndProcessWavFile(wavFileToClassify)
    );
  }

  std::pair<double,
            std::pair<std::string,
                      double>> classificationConfidenceAndProbability(std::string wavFiletoClassify) {
    return this->wisard
               .classificationConfidenceAndProbability(this->readAndProcessWavFile(wavFiletoClassify));
  }

 private:
  std::vector<char> readAndProcessWavFile(std::string wavFile) {
    WavHandler wavHandler(wavFile);
    PreProcessor preProcessor(wavHandler.getSampleRate());
    preProcessor.process(wavHandler.getAudioData());

    this->kernelCanvas.process(preProcessor.extractProcessedFrames());

    return this->kernelCanvas.getPaintedCanvas();
  }
};

}

#endif //DICTAWAV_DICTAWAV_H
