/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 10/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_KERNELCANVAS_H
#define DICTA_KERNELCANVAS_H

#include <cstddef>
#include <random>
#include <cmath>
#include <queue>
#include <utility>
#include <limits>
#include "../preprocessor/PreProcessor.h"
#include "Kernel.h"

namespace DictaWav
{
  template <class Frame>
  class KernelCanvas
  {
    private:
      std::size_t numKernels;
      std::size_t kernelDimension;
      int outputFactor = 1;
      std::vector<Kernel> kernels;
      std::vector<bool> activeKernels;
      std::vector<std::vector<double>> processedFrames;
    
    public:
      KernelCanvas(std::size_t numKernels, std::size_t kernelDimension, int outputFactor = 1) :
          numKernels(numKernels),
          kernelDimension(kernelDimension),
          activeKernels(numKernels, false),
          outputFactor(outputFactor)
      {
        for (int kernel = 0; kernel != numKernels; ++kernel)
          this->kernels.push_back(Kernel(this->kernelDimension * 4));
      }
      
      void preProcess(const std::vector<Frame>& frames)
      {
        // Cleaning the canvas
        this->processedFrames = std::vector<std::vector<double>>();
        this->cleanCanvas();
        
        this->appendSumFrames(frames);
        this->zScoreAndTanh();
        this->replicateFeatures();
      }
      
      std::vector<bool> getPaintedCanvas()
      {
        this->paintCanvas();
        
        auto activeKernelsSize = this->activeKernels.size();
        std::vector<bool> paintedCanvas;
        
        // KernelCanvas output can be replicated to give better results with WiSARD
        for (std::size_t index = 0; index != activeKernelsSize * this->outputFactor; ++index) {
          paintedCanvas.emplace_back(this->activeKernels[index % activeKernelsSize]);
        }
        return paintedCanvas;
      }
    
    private:
      void appendSumFrames(const std::vector<Frame>& frames)
      {
        // First frame
        auto& firstFrame = frames[0];
        std::vector<double> frame;
        for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension * 2; ++frameIndex) {
          frame.push_back(firstFrame[frameIndex % this->kernelDimension]);
        }
        this->processedFrames.emplace_back(frame);
        frame = std::vector<double>();
        
        // Other frames
        for (std::size_t index = 1; index != frames.size(); ++index) {
          auto& currentFrame = frames[index];
          
          for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
            frame.push_back(currentFrame[frameIndex]);
          
          for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
            frame.push_back(
                currentFrame[frameIndex]
                + this->processedFrames[index - 1][frameIndex + this->kernelDimension]
            );
          
          this->processedFrames.emplace_back(frame);
          frame = std::vector<double>();
        }
      }
      
      void zScoreAndTanh()
      {
        auto processed = std::vector<std::vector<double>>();
        auto doubledKernelDimension = this->kernelDimension * 2;
        
        std::vector<double> means(doubledKernelDimension);
        std::vector<double> standardDeviations(doubledKernelDimension);
        
        for (const auto& frame : this->processedFrames)
          for (std::size_t index = 0; index != doubledKernelDimension; ++index)
            means[index] += frame[index];
        
        for (auto& mean : means)
          mean /= this->processedFrames.size(); // Calculating mean for each dimension
        
        for (const auto& frame : this->processedFrames)
          for (std::size_t index = 0; index != doubledKernelDimension; ++index)
            standardDeviations[index] += std::pow((frame[index] - means[index]), 2);
        
        for (auto& standardDeviation : standardDeviations)
          // Calculating standard deviation for each dimension
          standardDeviation /= this->processedFrames.size() - 1;
        
        for (auto& frame: this->processedFrames) {
          std::vector<double> zScoredFrame(doubledKernelDimension);
          for (std::size_t index = 0; index != doubledKernelDimension; ++index)
            zScoredFrame[index] = std::tanh(
                (frame[index] - means[index]) / standardDeviations[index]
            ); // Applying Z-Score and Tanh
          processed.emplace_back(zScoredFrame);
        }
        
        this->processedFrames = processed;
      }
      
      void replicateFeatures()
      {
        auto doubledKernelDimension = this->kernelDimension * 2;
        // "Replicating features" on first frame just fill it with zeros
        for (std::size_t frameIndex = 0; frameIndex != doubledKernelDimension; ++frameIndex)
          this->processedFrames[0].push_back(0);
        
        for (std::size_t index = 1; index != this->processedFrames.size(); ++index)
          for (std::size_t frameIndex = 0; frameIndex != doubledKernelDimension; ++frameIndex)
            this->processedFrames[index].push_back(this->processedFrames[index - 1][frameIndex]);
      }
      
      std::size_t getNearestKernelIndex(const std::vector<double>& frame)
      {
        auto currentKernel = Kernel(this->kernelDimension * 4, frame);
        std::size_t nearestKernelIndex = 0;
        double nearestKernelDistance = std::numeric_limits<double>::max();
        
        for (std::size_t index = 0; index != this->kernels.size(); ++index) {
          auto distance = this->kernels[index].checkDistance(currentKernel);
          if (distance < nearestKernelDistance) {
            nearestKernelDistance = distance;
            nearestKernelIndex = index;
          }
        }
        
        return nearestKernelIndex;
      }
      
      void paintCanvas()
      {
        for (auto& frame : this->processedFrames) {
          this->activeKernels[this->getNearestKernelIndex(frame)] = true;
        }
      }
      
      void cleanCanvas()
      {
        for (auto iterator = this->activeKernels.begin();
             iterator != this->activeKernels.end(); ++iterator)
          *iterator = false;
      }
  };
}

#endif //DICTA_KERNELCANVAS_H
