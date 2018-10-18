/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_WISARD_H
#define DICTAWAV_WISARD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <cmath>
#include <memory>

#include "Discriminator.h"

namespace DictaWav {

class Wisard {

  size_t retinaSize;
  size_t ramNumBits;
  bool useBleaching;
  double minimumConfidence;
  unsigned bleachingThreshold;
  bool isCumulative;
  std::unordered_map<std::string, Discriminator> discriminators;
  std::shared_ptr<std::vector<size_t>> ramAddressMapping;

 public:
  Wisard(
      size_t retinaSize,
      size_t ramNumBits,
      bool useBleaching = true,
      double minimumConfidence = 0.002,
      unsigned bleachingThreshold = 1,
      bool randomizePositions = true,
      bool isCumulative = true
  ) :
      retinaSize(retinaSize),
      ramNumBits(ramNumBits),
      useBleaching(useBleaching),
      minimumConfidence(minimumConfidence),
      bleachingThreshold(bleachingThreshold),
      isCumulative(isCumulative),
      ramAddressMapping(std::make_shared<std::vector<size_t>>(retinaSize)) {

    for (size_t index = 0; index != retinaSize; ++index)
      (*(this->ramAddressMapping))[index] = index;

    if (randomizePositions)
      std::shuffle(
          this->ramAddressMapping->begin(),
          this->ramAddressMapping->end(),
          //          std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
          std::mt19937(std::random_device()())
      );
  }

  void train(const std::vector<char>& retina, const std::string& className) {
    // Checking if class name exists before creating a new discriminator
    if (this->discriminators.find(className) == this->discriminators.end())
      this->discriminators.insert({
                                      className, Discriminator(
              this->retinaSize,
              this->ramNumBits,
              this->ramAddressMapping,
              this->isCumulative
          )
                                  });

    // Training discriminator
    this->discriminators.at(className).train(retina);
  }
  void forget(const std::vector<char>& retina, const std::string& className) {
    if (this->discriminators.find(className) != this->discriminators.end())
      this->discriminators.at(className).forget(retina);
  }

  std::string classify(const std::vector<char>& retina) {
    return this->classificationConfidenceAndProbability(retina).second.first;
  }

  std::unordered_map<std::string, double> classificationsProbabilities(
      const std::vector<char>& retina
  ) {
    std::unordered_map<std::string, double> result(this->discriminators.size());
    std::unordered_map<std::string, std::vector<unsigned>> ramResults(this->discriminators.size());

    auto ramsCount = std::ceil(
        static_cast<double>(this->retinaSize) / static_cast<double>(this->ramNumBits)
    );

    // Testing with all discriminators
    for (const auto&[className, discriminator] : this->discriminators) {
      auto ramResult = discriminator.classify(retina);

      int positiveVotes = 0;
      for (size_t ramResultsIndex = 0; ramResultsIndex != ramResult.size(); ++ramResultsIndex)
        if (ramResult[ramResultsIndex] > 0)
          ++positiveVotes;

      // Calculating probability to see what percentage of rams recognize the element
      result[className] = static_cast<double>(positiveVotes) / ramsCount;
      ramResults[className] = ramResult;
    }

    if (this->useBleaching)
      result = this->applyBleaching(result, ramResults, ramsCount);

    return std::move(result);
  }

  std::pair<std::string, double> classificationAndProbability(
      const std::vector<char>& retina
  ) {
    return this->classificationConfidenceAndProbability(retina).second;
  }

  std::pair<double, std::pair<std::string, double>> classificationConfidenceAndProbability(
      const std::vector<char>& retina
  ) {
    auto result = this->calculateConfidence(this->classificationsProbabilities(retina));
    if (result.first < this->minimumConfidence) {
      return {0, {"Not enough confidence to decide", 0}};
    }

    return result;
  }

 private:
  std::unordered_map<std::string, double> applyBleaching(
      const std::unordered_map<std::string, double>& results,
      const std::unordered_map<std::string, std::vector<unsigned>>& ramResult,
      double ramsCount
  ) {
    std::unordered_map<std::string, double> bleachedResults = results;
    auto confidence = this->calculateConfidence(results).first;
    auto currentBleachingThreshold = this->bleachingThreshold;

    // First element in bestClass is confidence rating
    while (confidence < this->minimumConfidence) {

      double maxValue = 0.0;
      for (const auto&[className, result] : bleachedResults) {

        unsigned summedRamsValue = 0;
        for (size_t index = 0; index != ramResult.at(className).size(); ++index)
          if (ramResult.at(className)[index] > currentBleachingThreshold)
            ++summedRamsValue;

        bleachedResults[className] = static_cast<double>(summedRamsValue) / ramsCount;

        if ((result - maxValue) > 0.0001)
          maxValue = result;
      }

      // If no ram recognizes the pattern, return previous value
      if (maxValue <= 0.000001) {
        return results;
      }

      ++currentBleachingThreshold;
      confidence = this->calculateConfidence(bleachedResults).first;
    }

    return bleachedResults;
  }

  static std::pair<double, std::pair<std::string, double>> calculateConfidence(
      const std::unordered_map<std::string, double>& classificationsProbabilitiesResult
  ) {
    std::string bestClass;
    double max = 0.0;
    double secondMax = 0.0;

    for (const auto&[className, probability] : classificationsProbabilitiesResult) {
      if (max < probability) {
        secondMax = max;
        max = probability;
        bestClass = className;
      } else if (secondMax < probability)
        secondMax = probability;
    }

    double confidence = max != 0.0 ? 1.0 - secondMax / max : 0.0;
    // First value is confidence, second is a pair with best class name and it's probability
    return {confidence, {bestClass, max}};
  }

};

}

#endif //DICTAWAV_WISARD_H
