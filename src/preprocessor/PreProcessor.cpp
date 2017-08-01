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
              audioData->operator[](index) * this->hannWindowFunction(thirdFrameComplete.size()));
        
        firstFrame.push(audioData->operator[](index) * this->hannWindowFunction(firstFrame.size()));
      }
      else if (sampleCounter >= frameMidPoint && sampleCounter < this->samplesPerFrame) {
        if (!thirdFrameComplete.empty()) {
          this->addFrame(
              this->dftHandler.processDCT(
                  this->mfcc.computeMFCC(
                      this->dftHandler
                          .processFFT(thirdFrameComplete)
                  )
              )
          );
        }
        
        firstFrame.push(audioData->operator[](index) * this->hannWindowFunction(firstFrame.size()));
        secondFrame.push(
            audioData->operator[](index) * this->hannWindowFunction(secondFrame.size()));
      }
      else {
        secondFrame.push(
            audioData->operator[](index) * this->hannWindowFunction(secondFrame.size()));
        thirdFrameFirstHalf.push(
            audioData->operator[](index) * this->hannWindowFunction(thirdFrameFirstHalf.size()));
      }
      
      thirdFrameComplete = std::move(thirdFrameFirstHalf);
      this->addFrame(
          this->dftHandler
              .processDCT(
                  this->mfcc
                      .computeMFCC(this->dftHandler.processFFT(firstFrame))));
      this->addFrame(
          this->dftHandler
              .processDCT(
                  this->mfcc
                      .computeMFCC(this->dftHandler.processFFT(secondFrame))));
      
      if (sampleCounter > this->samplesPerFrame + frameMidPoint)
        sampleCounter = 0;
      else
        ++sampleCounter;
    }
  }
  
//  void PreProcessor::report() // Execute on terminal: graph -T png -C --bitmap-size 4000x4000 < A.txt > plot.png
//  {
//    while (true) {
//      if (!this->processedFrames.empty()) {
//        auto& frame = this->processedFrames.front();
//        for (int pos = 0; pos != frame.size(); ++pos)
//          std::cout << pos << " " << frame[pos] << "\n";
//
//        std::cout << "\n";
//        this->processedFrames.pop();
//      }
//    }
//  }
}