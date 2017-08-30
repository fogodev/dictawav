/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/preprocessor/PreProcessor.h"

namespace DictaWav
{
  void PreProcessor::process(std::shared_ptr<std::vector<double>> audioData)
  {
    auto frameMidPoint = this->samplesPerFrame / 2;
    Frame<double> firstFrame(this->samplesPerFrame);
    Frame<double> secondFrame(this->samplesPerFrame);
    Frame<double> thirdFrameFirstHalf(this->samplesPerFrame);
    Frame<double> thirdFrameComplete;
    std::size_t sampleCounter = 0;
    
    for (std::size_t index = 0; index != audioData->size(); ++index) {
      if (sampleCounter < frameMidPoint) {
        if (!thirdFrameComplete.empty())
          thirdFrameComplete.push(
              (*audioData)[index] * this->hannWindowFunction(thirdFrameComplete.size()));
        
        firstFrame.push((*audioData)[index] * this->hannWindowFunction(firstFrame.size()));
      }
      else if (sampleCounter >= frameMidPoint && sampleCounter < this->samplesPerFrame) {
        if (!thirdFrameComplete.empty() && thirdFrameComplete.size() == this->samplesPerFrame) {
          this->addFrame(this->dftHandler.processDCT(
                  this->mfcc.computeMFCC(this->dftHandler.processFFT(thirdFrameComplete))
          ));
        }
        
        firstFrame.push((*audioData)[index] * this->hannWindowFunction(firstFrame.size()));
        secondFrame.push(
            (*audioData)[index] * this->hannWindowFunction(secondFrame.size()));
      }
      else {
        secondFrame.push(
            (*audioData)[index] * this->hannWindowFunction(secondFrame.size()));
        thirdFrameFirstHalf.push(
            (*audioData)[index] * this->hannWindowFunction(thirdFrameFirstHalf.size()));
      }
      
      if(thirdFrameFirstHalf.size() == frameMidPoint){
        thirdFrameComplete = std::move(thirdFrameFirstHalf);
        thirdFrameFirstHalf = Frame<double>(this->samplesPerFrame);
      }
      if(firstFrame.size() == this->samplesPerFrame){
        this->addFrame(this->dftHandler.processDCT(
            this->mfcc.computeMFCC(this->dftHandler.processFFT(firstFrame))
        ));
        firstFrame = Frame<double>(this->samplesPerFrame);
      }
  
      if(secondFrame.size() == this->samplesPerFrame){
        this->addFrame(this->dftHandler.processDCT(
            this->mfcc.computeMFCC(this->dftHandler.processFFT(secondFrame))
        ));
        secondFrame = Frame<double>(this->samplesPerFrame);
      }
      
      if (sampleCounter > this->samplesPerFrame + frameMidPoint)
        sampleCounter = 0;
      else
        ++sampleCounter;
    }
    
    // Adding remaining frames
    if(!firstFrame.empty()){
      while(firstFrame.size() < this->samplesPerFrame)
        firstFrame.push(0);
      this->addFrame(this->dftHandler.processDCT(
          this->mfcc.computeMFCC(this->dftHandler.processFFT(firstFrame))
      ));
    }
    
    if(!secondFrame.empty()){
      while(secondFrame.size() < this->samplesPerFrame)
        secondFrame.push(0);
      this->addFrame(this->dftHandler.processDCT(
          this->mfcc.computeMFCC(this->dftHandler.processFFT(secondFrame))
      ));
    }
  }
}