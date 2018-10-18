/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 09/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_MFCC_H
#define DICTA_MFCC_H

#include <cmath>
#include <vector>
#include <array>
#include "DCTHandler.h"

namespace DictaWav {

class MFCC {
  size_t filterBanksCount;
  size_t sampleRate;
  size_t frameSize;
  double lowestFrequency;
  double highestFrequency;
  DCTHandler dctHandler;
  std::vector<std::array<size_t, 3>> filterBanks;

 public:
  MFCC(
      size_t filterBanksCount,
      size_t sampleRate,
      size_t frameLength,
      double lowerFrequency,
      double higherFrequency
  ) :
      filterBanksCount(filterBanksCount),
      sampleRate(sampleRate),
      frameSize(frameLength),
      lowestFrequency(lowerFrequency),
      highestFrequency(higherFrequency),
      dctHandler(filterBanksCount) {
    this->filterBanks.reserve(filterBanksCount);
    this->createFilterBanks();
  }

  std::vector<double> compute(const std::vector<double>& frame) {
    std::vector<double> filteredValues(this->filterBanksCount);

    int currentFilter = 0;
    for (const auto& filterBank : this->filterBanks) {
      auto begin = filterBank[0];
      auto mid = filterBank[1];
      auto end = filterBank[2];

      for (auto pos = begin; pos != mid; ++pos)
        filteredValues[currentFilter] +=
            frame[pos] * static_cast<double>(pos - begin) / static_cast<double>(mid - begin);
      for (auto pos = mid; pos != end; ++pos)
        filteredValues[currentFilter] +=
            frame[pos] * static_cast<double>(end - pos) / static_cast<double>(end - mid);

      ++currentFilter;
    }
    for (int pos = 0; pos != this->filterBanksCount; ++pos)
      filteredValues[pos] = std::log(filteredValues[pos]);

    return this->dctHandler.process(filteredValues);
  }

 private:
  void createFilterBanks() {
    double lowerMel = this->hertzToMels(this->lowestFrequency);
    double higherMel = this->hertzToMels(this->highestFrequency);
    double deltaMel = (higherMel - lowerMel) / (this->filterBanksCount + 1.0);

    std::vector<size_t> bins;
    bins.reserve(this->filterBanksCount + 2);

    for (size_t pos = 0; pos != this->filterBanksCount + 2; ++pos) {
      bins.push_back(
          static_cast<size_t>(
              std::floor((this->frameSize + 1.0) * this->melsToHertz(lowerMel + pos * deltaMel)
                             / static_cast<double>(this->sampleRate)
              )));
    }

    for (size_t pos = 0; pos != this->filterBanksCount; ++pos) {
      std::array<size_t, 3> currentFilterBank{bins[pos], bins[pos + 1], bins[pos + 2]};
      this->filterBanks.push_back(currentFilterBank);
    }
  }

  double hertzToMels(double hertz) { return 1127.0 * std::log(1.0 + hertz / 700.0); }

  double melsToHertz(double mels) { return 700.0 * (std::exp(mels / 1127.0) - 1.0); }
};
}

#endif //DICTA_MFCC_H
