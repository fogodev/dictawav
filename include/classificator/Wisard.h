/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_WISARD_H
#define DICTAWAV_WISARD_H

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

#include "Discriminator.h"

namespace DictaWav
{
  class Wisard
  {
    private:
      std::size_t retinaSize;
      std::size_t numBitsAddress;
      bool useBleaching;
      double confidenceMinimumRate;
      unsigned bleachingThreshold;
      bool randomizePositions;
      bool isCumulative;
      std::unordered_map<std::string, Discriminator*> discriminators;
      std::vector<std::size_t> ramAddressMapping;
    
    public:
      Wisard(
          std::size_t retinaSize,
          std::size_t numBitsAddr,
          bool useBleaching = true,
          double confidenceMinimumRate = 0.1,
          unsigned bleachingThreshold = 1,
          bool randomizePositions = true,
          bool isCumulative = true
            );
      ~Wisard();
      
      void train(std::vector<bool> trainingRetina, const std::string& className);
      
      std::string classify(std::vector<bool> retina);
      
      std::unordered_map<std::string, double> classificationsProbabilities(
          std::vector<bool> retina
                                                                            );
      std::pair<std::string, double> classificationAndProbability(
          std::vector<bool> retina
                                                                   );
      
      std::pair<double, std::pair<std::string, double>> classificationConfidenceAndProbability(
          std::vector<bool> retina
                                                                                              );
    
    private:
      std::unordered_map<std::string, double> applyBleaching(
          std::unordered_map<std::string, double>& classificationsProbabilitiesResult,
          std::unordered_map<std::string, std::vector<unsigned>>& ramResult
                                                              );
      
      std::pair<double, std::pair<std::string, double>> calculateConfidence(
          const std::unordered_map<std::string, double>& classificationsProbabilitiesResult
                                                                             );
  };
}

#endif //DICTAWAV_WISARD_H
