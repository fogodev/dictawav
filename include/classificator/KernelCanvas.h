/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 10/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_KERNELCANVAS_H
#define DICTA_KERNELCANVAS_H

#include <random>
#include <cmath>
#include <limits>
#include "Kernel.h"

namespace DictaWav {

class KernelCanvas {
 private:
  size_t numKernels;
  size_t kernelDimension;
  int outputFactor;
  std::vector<Kernel> kernels{};
  std::vector<char> activeKernels{};
  std::vector<std::vector<double>> processedFrames{};

 public:
  KernelCanvas(size_t numKernels, size_t kernelDimension, int outputFactor = 1) :
      numKernels(numKernels),
      kernelDimension(kernelDimension),
      activeKernels(numKernels, false),
      outputFactor(outputFactor) {
    this->kernels.reserve(numKernels);
    for (size_t kernel = 0; kernel != numKernels; ++kernel)
      this->kernels.emplace_back(Kernel(kernelDimension * 4));
  }

  void process(const std::vector<std::vector<double>>& frames) {
    // Cleaning current canvas
    this->processedFrames = std::vector<std::vector<double>>();

    this->appendSumFrames(frames);
    this->zScoreAndTanh();
    this->replicateFeatures();
  }

  std::vector<char> getPaintedCanvas() {
    this->paintCanvas();

    std::vector<char> paintedCanvas;
    paintedCanvas.reserve(this->numKernels * this->outputFactor);

    // KernelCanvas output can be replicated to give better results with WiSARD
    for (size_t output = 0; output != this->outputFactor; ++output)
      for (size_t index = 0; index != this->activeKernels.size(); ++index)
        paintedCanvas.push_back(this->activeKernels[index]);

    this->cleanCanvas();
    return paintedCanvas;
  }

 private:
  void appendSumFrames(const std::vector<std::vector<double>>& frames) {
    // First frame
    auto& firstFrame = frames[0];
    std::vector<double> frame;
    frame.reserve(this->kernelDimension * 2);
    for (size_t doubling = 0; doubling != 2; ++doubling) {
      for (size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex) {
        frame.push_back(firstFrame[frameIndex]);
      }
    }

    this->processedFrames.emplace_back(frame);
    frame = std::vector<double>();
    frame.reserve(this->kernelDimension * 2);

    // Other frames
    for (size_t index = 1; index != frames.size(); ++index) {
      auto& currentFrame = frames[index];

      for (size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
        frame.push_back(currentFrame[frameIndex]);

      for (size_t frameIndex = 0; frameIndex != this->kernelDimension; ++frameIndex)
        frame.push_back(
            currentFrame[frameIndex]
                + this->processedFrames[index - 1][frameIndex + this->kernelDimension]
        );

      this->processedFrames.emplace_back(frame);
      frame = std::vector<double>();
      frame.reserve(this->kernelDimension * 2);
    }
  }

  void zScoreAndTanh() {
    const auto processedFramesCount = this->processedFrames.size();
    auto doubledKernelDimension = this->kernelDimension * 2;
    auto processed = std::vector<std::vector<double>>();
    processed.reserve(processedFramesCount);

    std::vector<double> means(doubledKernelDimension);
    std::vector<double> standardDeviations(doubledKernelDimension);

    for (const auto& frame : this->processedFrames)
      for (size_t index = 0; index != doubledKernelDimension; ++index)
        means[index] += frame[index];

    for (auto& mean : means)
      mean /= static_cast<double>(processedFramesCount); // Calculating mean for each dimension

    for (const auto& frame : this->processedFrames)
      for (size_t index = 0; index != doubledKernelDimension; ++index) {
        const double current = frame[index] - means[index];
        standardDeviations[index] += current * current;
      }

    for (auto& standardDeviation : standardDeviations)
      // Calculating standard deviation for each dimension
      standardDeviation /= static_cast<double>(processedFramesCount - 1);

    for (auto& frame: this->processedFrames) {
      std::vector<double> zScoredFrame;
      zScoredFrame.reserve(doubledKernelDimension);

      // Applying Z-Score and Tanh
      for (size_t index = 0; index != doubledKernelDimension; ++index) {
        zScoredFrame.push_back(
            std::tanh((frame[index] - means[index]) / standardDeviations[index]));
      }

      processed.push_back(zScoredFrame);
    }

    this->processedFrames = processed;
  }

  void replicateFeatures() {
    size_t doubledKernelDimension = this->kernelDimension * 2;
    // "Replicating features" on first frame just fill it with zeros
    for (size_t frameIndex = 0; frameIndex != doubledKernelDimension; ++frameIndex)
      this->processedFrames[0].push_back(0.0);

    for (size_t index = 1; index != this->processedFrames.size(); ++index)
      for (size_t frameIndex = 0; frameIndex != doubledKernelDimension; ++frameIndex)
        this->processedFrames[index].push_back(this->processedFrames[index - 1][frameIndex]);
  }

  size_t getNearestKernelIndex(const std::vector<double>& frame) {
    auto currentKernel = Kernel(this->kernelDimension * 4, frame);
    size_t nearestKernelIndex = 0;
    double nearestKernelDistance = std::numeric_limits<double>::max();

    for (size_t index = 0; index != this->numKernels; ++index) {
      auto distance = this->kernels[index].checkDistanceSquared(currentKernel);
      if (distance < nearestKernelDistance) {
        nearestKernelDistance = distance;
        nearestKernelIndex = index;
      }
    }

    return nearestKernelIndex;
  }

  void paintCanvas() {
    for (auto& frame : this->processedFrames) {
      this->activeKernels[this->getNearestKernelIndex(frame)] = true;
    }
  }

  void cleanCanvas() {
    for (auto& active : this->activeKernels)
      active = false;
  }
};

}

#endif //DICTA_KERNELCANVAS_H
