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
      std::vector<Kernel> kernels;
      std::vector<std::vector<double>> processedFrames;
    
    public:
      KernelCanvas(std::size_t numKernels, std::size_t kernelDimension) :
          numKernels(numKernels),
          kernelDimension(kernelDimension)
      {
        for (int kernel = 0; kernel != numKernels; ++kernel)
          this->kernels.push_back(Kernel(this->kernelDimension * 4));
      }
      
      void preProcess(std::vector<Frame>&& frames)
      {
        this->appendSumFrames(std::move(frames));
        this->zScoreAndTanh();
        this->replicateFeatures();
      }
      
      void paintCanvas()
      {
        for (const auto& frame : this->processedFrames) {
        
        }
      }
    
    private:
      void appendSumFrames(std::vector<Frame>&& frames)
      {
        std::vector<std::vector<double>> processed;
        
        // First frame
        auto& firstFrame = frames[0];
        std::vector<double> frame;
        for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension * 2; ++frameIndex) {
          frame.push_back(firstFrame[frameIndex % this->kernelDimension]);
        }
        processed.push_back(std::move(frame));
        frame = std::vector<double>();
        
        // Other frames
        for (std::size_t index = 1; index != frames.size(); ++index) {
          auto& currentFrame = frames[index];
          
          for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
            frame.push_back(currentFrame[frameIndex]);
          
          for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
            frame.push_back(
                currentFrame[frameIndex] + processed[index - 1][frameIndex + this->kernelDimension]
            );
          
          processed.push_back(std::move(frame));
          frame = std::vector<double>();
        }
        
        // Doubling up frame size
        this->kernelDimension *= 2;
        
        std::swap(this->processedFrames, processed);
      }
      
      void zScoreAndTanh()
      {
        std::vector<std::vector<double>> processed;
        
        std::vector<double> means(this->kernelDimension);
        std::vector<double> standardDeviations(this->kernelDimension);
        
        for (const auto& frame : this->processedFrames)
          for (std::size_t index = 0; index != this->kernelDimension; ++index)
            means[index] += frame[index];
        
        for (auto& mean : means)
          mean /= this->processedFrames.size(); // Calculating mean for each dimension
        
        for (const auto& frame : this->processedFrames)
          for (std::size_t index = 0; index != this->kernelDimension; ++index)
            standardDeviations[index] += std::pow((frame[index] - means[index]), 2);
        
        for (auto& standardDeviation : standardDeviations)
          // Calculating standard deviation for each dimension
          standardDeviation /= this->processedFrames.size() - 1;
        
        for (auto& frame: this->processedFrames) {
          std::vector<double> zScoredFrame(this->kernelDimension);
          for (std::size_t index = 0; index != this->kernelDimension; ++index)
            zScoredFrame[index] = std::tanh(
                (frame[index] - means[index]) / standardDeviations[index]
            ); // Applying Z-Score and Tanh
          processed.push_back(zScoredFrame);
        }
        
        std::swap(this->processedFrames, processed);
      }
      
      void replicateFeatures()
      {
        // "Replicating features" on first frame just fill it with zeros
        for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
          this->processedFrames[0].push_back(0);
        
        for (std::size_t index = 1; index != this->processedFrames.size(); ++index)
          for (std::size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
            this->processedFrames[index].push_back(this->processedFrames[index - 1][frameIndex]);
        
        // Doubling up frame size
        this->kernelDimension *= 2;
      }
  };
}

#endif //DICTA_KERNELCANVAS_H
