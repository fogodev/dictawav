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
#include "Frame.hpp"

namespace DictaWav
{
  template <class T>
  class MFCC
  {
    private:
      std::size_t sampleRate;
      std::size_t filterBanksCount;
      std::size_t frameLength;
      T lowerFrequency;
      T higherFrequency = sampleRate / 2.0;
      std::vector<std::array<int, 3>> filterBanks;
    
    public:
      MFCC(
          std::size_t sampleRate,
          std::size_t filterBanksCount,
          std::size_t frameLength,
          T lowerFrequency,
          T higherFrequency
          ) :
          sampleRate(sampleRate),
          filterBanksCount(filterBanksCount),
          frameLength(frameLength),
          lowerFrequency(lowerFrequency),
          higherFrequency(higherFrequency)
      {
        this->createFilterBanks();
      }
      
      Frame<T> computeMFCC(const Frame<T>& frame)
      {
        Frame<T> filteredFrame(this->filterBanksCount);
        T* filteredValues = new T[this->filterBanksCount]();
        
        int currentFilter = 0;
        for (const auto& filterBank : this->filterBanks) {
          auto begin = filterBank[0];
          auto center = filterBank[1];
          auto end = filterBank[2];
          
          for (auto pos = begin; pos != center; ++pos)
            filteredValues[currentFilter] +=
                frame[pos] * (pos - begin) / static_cast<T>(center - begin);
          for (auto pos = center; pos != end; ++pos)
            filteredValues[currentFilter] +=
                frame[pos] * (end - pos) / static_cast<T>(end - center);
          
          ++currentFilter;
        }
        for (int pos = 0; pos != this->filterBanksCount; ++pos)
          filteredFrame.push(std::log(filteredValues[pos]));
        
        delete[] filteredValues;
        
        return filteredFrame;
      }
    
    private:
      void createFilterBanks()
      {
        T lowerMel = this->hertzToMels(this->lowerFrequency);
        T higherMel = this->hertzToMels(this->higherFrequency);
        T deltaMel = (higherMel - lowerMel) / (this->filterBanksCount + 1);
        
        std::vector<int> bins;
        
        for (int pos = 0; pos != this->filterBanksCount + 2; ++pos) {
          bins.push_back(
              std::floor((this->frameLength + 1) * this->melsToHertz(lowerMel + pos * deltaMel)
                         / this->sampleRate
              ));
        }
        
        for (int pos = 0; pos != this->filterBanksCount; ++pos) {
          std::array<int, 3> currentFilterBank;
          currentFilterBank[0] = bins[pos];
          currentFilterBank[1] = bins[pos + 1];
          currentFilterBank[2] = bins[pos + 2];
          this->filterBanks.push_back(currentFilterBank);
        }
      }
      
      T hertzToMels(T hertz)
      { return 1127 * std::log(1 + hertz / 700); }
      
      T melsToHertz(T mels)
      { return 700 * (std::exp(mels / 1127) - 1); }
  };
}

#endif //DICTA_MFCC_H
