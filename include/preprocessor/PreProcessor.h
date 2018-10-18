/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_PREPROCESSOR_H
#define DICTA_PREPROCESSOR_H

#include <queue>
#include <cmath>
#include <iostream>
#include <memory>
#include "FFTHandler.h"
#include "MFCC.h"

using Frame = std::vector<double>;

namespace DictaWav {

class PreProcessor {
  size_t samplesPerFrame;
  std::vector<Frame> processedFrames;
  FFTHandler fftHandler;
  MFCC mfcc;

  static constexpr size_t filterBankCount = 26;
  static constexpr size_t lowestFrequency = 0;
  static constexpr double pi = 3.14159265358979323846;

 public:
  explicit PreProcessor(size_t sampleRate) :
      samplesPerFrame(getNextPowerOf2(sampleRate / 50)), // To get 20ms sized Frames
      fftHandler(samplesPerFrame),
      mfcc(
          filterBankCount,
          sampleRate,
          samplesPerFrame,
          lowestFrequency,
          getHighestFrequency(sampleRate)
      ) {}

  void process(const std::vector<double>& audioData) {
    auto frameMidPoint = this->samplesPerFrame / 2;

    Frame firstFrame;
    firstFrame.reserve(this->samplesPerFrame);

    Frame secondFrame;
    secondFrame.reserve(this->samplesPerFrame);

    Frame thirdFrameFirstHalf;
    thirdFrameFirstHalf.reserve(this->samplesPerFrame);

    Frame thirdFrameComplete;
    size_t sampleCounter = 0;

    for (const auto sample : audioData) {
      if (sampleCounter < frameMidPoint) {
        if (!thirdFrameComplete.empty()) {
          this->pushWindowedSample(sample, thirdFrameComplete);
        }

        this->pushWindowedSample(sample, firstFrame);
      } else if (sampleCounter >= frameMidPoint && sampleCounter < this->samplesPerFrame) {
        if (!thirdFrameComplete.empty() && thirdFrameComplete.size() == this->samplesPerFrame) {
          this->processAndAddFrame(thirdFrameComplete);
          thirdFrameComplete = std::move(Frame{}); // (this->samplesPerFrame);
        }

        this->pushWindowedSample(sample, firstFrame);
        this->pushWindowedSample(sample, secondFrame);
      } else {
        this->pushWindowedSample(sample, secondFrame);
        this->pushWindowedSample(sample, thirdFrameFirstHalf);
      }

      if (thirdFrameFirstHalf.size() == frameMidPoint) {
        thirdFrameComplete = std::move(thirdFrameFirstHalf);
        thirdFrameFirstHalf = std::move(Frame{});
        thirdFrameFirstHalf.reserve(this->samplesPerFrame);
      }

      if (firstFrame.size() == this->samplesPerFrame) {
        this->processAndAddFrame(firstFrame);
        firstFrame = std::move(Frame{});
        firstFrame.reserve(this->samplesPerFrame);
      }

      if (secondFrame.size() == this->samplesPerFrame) {
        this->processAndAddFrame(secondFrame);
        secondFrame = std::move(Frame{});
        secondFrame.reserve(this->samplesPerFrame);
      }

      if (sampleCounter > this->samplesPerFrame + frameMidPoint)
        sampleCounter = 0;
      else
        ++sampleCounter;
    }

    // Adding remaining frames
    this->checkFillAndAddIncompleteFrame(firstFrame);
    this->checkFillAndAddIncompleteFrame(secondFrame);
    this->checkFillAndAddIncompleteFrame(thirdFrameFirstHalf);
    this->checkFillAndAddIncompleteFrame(thirdFrameComplete);
  }

  std::vector<Frame> extractProcessedFrames() {
    return std::move(this->processedFrames);
  }

  void processAndAddFrame(Frame& frame) {
    this->processedFrames.push_back(
        this->mfcc.compute(
            this->fftHandler.process(frame)
        )
    );
  }

  void pushWindowedSample(double sample, Frame& frame) {
    auto length = frame.size();
    frame.push_back(sample * this->hannWindowFunction(length));
  }

  void checkFillAndAddIncompleteFrame(Frame& frame) {
    if (!frame.empty()) {
      while (frame.size() < this->samplesPerFrame)
        frame.push_back(0.0);
      this->processAndAddFrame(frame);
    }
  }

  double hannWindowFunction(size_t index) {
    return 0.5 * (1.0 - std::cos((2.0 * pi * index) / static_cast<double>(this->samplesPerFrame)));
  }

 private:
  constexpr size_t getNextPowerOf2(size_t num) {
    size_t base2 = 1;
    while (base2 <= num)
      base2 <<= 1;
    return base2;
  }

  constexpr double getHighestFrequency(size_t sampleRate) {
    return static_cast<double>(sampleRate) / 2.0;
  }
};
}
#endif //DICTA_PREPROCESSOR_H
