/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/


#include "../../include/classificator/Wisard.h"

namespace DictaWav
{
  Wisard::Wisard(
      std::size_t retinaSize,
      std::size_t numBitsAddr,
      bool useBleaching = true,
      double confidenceMinimumRate = 0.1,
      unsigned bleachingThreshold = 1,
      bool randomizePositions = true,
      bool isCumulative = true
                ) :
      retinaSize(retinaSize),
      numBitsAddress(numBitsAddr),
      useBleaching(useBleaching),
      confidenceMinimumRate(confidenceMinimumRate),
      bleachingThreshold(bleachingThreshold),
      randomizePositions(randomizePositions),
      isCumulative(isCumulative)
  {
    for (std::size_t index = 0; index != retinaSize; ++index)
      this->ramAddressMapping.push_back(index);
    
    if (this->randomizePositions)
      std::shuffle(
          this->ramAddressMapping.begin(),
          this->ramAddressMapping.end(),
          std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
      );
  }
  
  Wisard::~Wisard()
  {
    for (auto iterator = this->discriminators.begin();
         iterator != this->discriminators.end(); ++iterator)
      delete iterator->second;
  }
  
  void Wisard::train(
      const std::vector<bool>& trainingRetina,
      const std::string& className
                    )
  {
    // Checking if class name exists before creating a new discriminator
    if (this->discriminators.find(className) == this->discriminators.end())
      this->discriminators[className] = new Discriminator(
          this->retinaSize,
          this->numBitsAddress,
          this->ramAddressMapping,
          this->isCumulative
      );
    
    // Training discriminator
    this->discriminators[className]->train(trainingRetina);
  }
  
  std::unordered_map<std::string, double>&& Wisard::classificationsProbabilities(
      const std::vector<bool>& retina
                                                                                )
  {
    std::unordered_map<std::string, double> result;
    std::unordered_map<std::string, std::vector<unsigned>> ramResults;
    
    std::size_t ramsCount = static_cast<std::size_t>(
        std::ceil(
            static_cast<double>(this->retinaSize) / static_cast<double>(this->numBitsAddress)
        )
    );
    
    // Testing with all discriminators
    for (auto iterator = this->discriminators.begin();
         iterator != this->discriminators.end(); ++iterator) {
      const auto& label = iterator->first;
      auto discriminator = iterator->second;
      auto ramResult = discriminator->classify(retina);
      
      int summedRamsValue = 0;
      for (std::size_t ramResultsIndex = 0; ramResultsIndex != ramResult.size(); ++ramResultsIndex)
        if (ramResult[ramResultsIndex] > 0)
          ++summedRamsValue;
      
      // Calculating probability to see what percentage of rams recognize the element
      result[label] = static_cast<double>(summedRamsValue) / static_cast<double>(ramsCount);
      ramResults[label] = ramResult;
    }
    
    if (this->useBleaching)
      result = this->applyBleaching(result, ramResults);
    
    return std::move(result);
  }
  
  std::string Wisard::classify(const std::vector<bool>& retina)
  {
    return this->classificationConfidenceAndProbability(retina).second.first;
  }
  
  std::pair<std::string, double>&& Wisard::classificationAndProbability(
      const std::vector<bool>& retina
                                                                       )
  {
    return std::move(this->classificationConfidenceAndProbability(retina).second);
  }
  
  std::pair<double, std::pair<std::string, double>>&& Wisard::classificationConfidenceAndProbability(
      const std::vector<bool>& retina
                                                                                                    )
  {
    auto result = this->calculateConfidence(this->classificationsProbabilities(retina));
    if (result.first < this->confidenceMinimumRate)
      return std::move(std::make_pair(0, std::make_pair("Not enough confidence to decide", 0)));
    
    return std::move(result);
  }
  
  std::unordered_map<std::string, double>&& Wisard::applyBleaching(
      std::unordered_map<std::string, double>& classificationsProbabilitiesResult,
      std::unordered_map<std::string, std::vector<unsigned>>& ramResult
                                                                  )
  {
    auto bleachingThreshold = this->bleachingThreshold;
    auto classifications = classificationsProbabilitiesResult;
    
    auto ramCount = static_cast<std::size_t>(std::ceil(
        static_cast<double>(this->retinaSize) / static_cast<double>(this->numBitsAddress)));
    
    auto confidence = this->calculateConfidence(classificationsProbabilitiesResult).first;
    
    // First element in bestClass is confidence rating
    while (confidence < this->confidenceMinimumRate) {
      
      double maxValue = 0.0;
      for (const auto& result : classifications) {
        unsigned summedRamsValue = 0;
        
        for (std::size_t index = 0; index != ramResult[result.first].size(); ++index)
          if (ramResult[result.first][index] > bleachingThreshold)
            ++summedRamsValue;
        
        classifications[result.first] =
            static_cast<double>(summedRamsValue) / static_cast<double>(ramCount);
        
        if ((result.second - maxValue) > 0.0001)
          maxValue = result.second;
      }
      
      // If no ram recognizes the pattern, return previous value
      if (maxValue <= 0.000001) {
        classifications = classificationsProbabilitiesResult;
        break;
      }
      
      ++bleachingThreshold;
      confidence = this->calculateConfidence(classifications).first;
    }
    
    return std::move(classifications);
  }
  
  std::pair<double, std::pair<std::string, double>>&& Wisard::calculateConfidence(
      const std::unordered_map<std::string, double>& classificationsProbabilitiesResult
                                                                                 )
  {
    std::pair<std::string, double> bestClass;
    double max = 0.0;
    double secondMax = 0.0;
    
    for (const auto& result : classificationsProbabilitiesResult) {
      auto value = result.second;
      if (max < value) {
        secondMax = max;
        max = value;
        bestClass.first = result.first;
        bestClass.second = result.second;
      }
      else if (secondMax < value)
        secondMax = value;
    }
    
    // First value is confidence, second is a pair with best class name and it's probability
    return std::move(std::make_pair((max - secondMax) / max, bestClass));
  }
}