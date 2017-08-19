/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 13/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../include/dictawav.h"

namespace DictaWav
{
  void DictaWav::readAndProcessWavFile(std::string wavFile)
  {
    this->wavHandlerPtr = std::make_unique<WavHandler>(wavFile);
    this->preProcessorPtr = std::make_unique<PreProcessor>(this->wavHandlerPtr->getSampleRate());
    this->preProcessorPtr->process(this->wavHandlerPtr->getAudioData());
    this->kernelCanvas.preProcess(this->preProcessorPtr->getFrames());
  }
  
  void DictaWav::newTraining(std::string wavTrainingFile, std::string className)
  {
    this->readAndProcessWavFile(wavTrainingFile);
    this->wisard.train(this->kernelCanvas.getPaintedCanvas(), className);
  }
  
  std::string DictaWav::classify(std::string wavFileToClassify)
  {
    this->readAndProcessWavFile(wavFileToClassify);
    return this->wisard.classify(this->kernelCanvas.getPaintedCanvas());
  }
}
