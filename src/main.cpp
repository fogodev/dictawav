#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include "../include/dictawav.h"

// KernelCanvas parameters
const size_t kernelCanvasNumKernels = 2048;
const size_t kernelCanvasKernelDimension = 13;
const int kernelCanvasOutputFactor = 10;

// WiSARD parameters
const size_t wisardRetinaSize = kernelCanvasNumKernels * kernelCanvasOutputFactor;
const size_t wisardNumBitsAddr = 32;
const bool wisardUseBleaching = true;
const double wisardConfidenceMinimumRate = 0.002;
const unsigned wisardBleachingThreshold = 1;
const bool wisardRandomizePositions = true;
const bool wisardIsCumulative = true;

double runTestsKfold(std::unordered_map<std::string,
                                        std::unordered_set<std::string>> classificationPaths);

int main(int argc, char** argv) {
  std::vector<std::string> words{
      "a", "ah!", "ai!", "ainda", "ano", "assim", "até", "au!",
      "bom", "casa", "cem", "certo", "coisa", "com", "como", "dar",
      "de", "depois", "dia", "dois", "e", "ele", "em", "esse",
      "estar", "este", "eu", "fazer", "ficar", "grande", "haver", "homem",
      "hum!", "ir", "isso", "já", "mais", "mas", "melhor", "mesmo",
      "mil", "moça", "moço", "muito", "não", "o", "ou", "para",
      "pior", "poder", "por", "porque", "primeiro", "próprio", "quando", "que",
      "se", "senhor", "senhora", "ser", "sua", "também", "tempo", "ter",
      "ui!", "último", "um", "uma", "ver", "vez", "você"
  };

  std::unordered_map<std::string, std::unordered_set<std::string>> classificationPaths;

  for (const auto& word : words) {
    for (size_t fileNumber = 1; fileNumber != 6; ++fileNumber) {
      std::filesystem::path path(std::filesystem::current_path());
      path /= "dataset";
      path /= word;
      path /= std::to_string(fileNumber) + ".wav";

      if (classificationPaths.find(word) == classificationPaths.end())
        classificationPaths[word] = std::unordered_set<std::string>{5};

      classificationPaths[word].insert(path.string());
    }
  }

  std::vector<double> accuracies;
  size_t numTests = 10;
  for (size_t i = 0; i != numTests; ++i) {
    accuracies.push_back(runTestsKfold(classificationPaths));
  }

  double mean = 0.0;
  for (const auto& accuracy : accuracies)
    mean += accuracy;
  mean /= static_cast<double>(numTests);

  double standardDeviation = 0.0;
  for (const auto& accuracy : accuracies)
    standardDeviation += (accuracy - mean) * (accuracy - mean);

  standardDeviation /= static_cast<double>(numTests);
  standardDeviation = std::sqrt(standardDeviation);

  std::cout << "Total accuracy on " << numTests << " tests: "
            << 100.0 * mean << "%" << std::endl;
  std::cout << "Standard deviation on " << numTests << " tests: "
            << 100.0 * standardDeviation << std::endl;

  return 0;
}

double runTestsKfold(std::unordered_map<std::string,
                                        std::unordered_set<std::string>> classificationPaths) {

  DictaWav::DictaWav dictaWav{
      kernelCanvasNumKernels,
      kernelCanvasKernelDimension,
      kernelCanvasOutputFactor,
      wisardRetinaSize,
      wisardNumBitsAddr,
      wisardUseBleaching,
      wisardConfidenceMinimumRate,
      wisardBleachingThreshold,
      wisardRandomizePositions,
      wisardIsCumulative
  };

  auto totalWordsPerFold = classificationPaths.size();

  // 5 folds, each one with 1 path from each word
  std::vector<std::unordered_map<std::string, std::string>> folds{
      std::unordered_map<std::string, std::string>{totalWordsPerFold},
      std::unordered_map<std::string, std::string>{totalWordsPerFold},
      std::unordered_map<std::string, std::string>{totalWordsPerFold},
      std::unordered_map<std::string, std::string>{totalWordsPerFold},
      std::unordered_map<std::string, std::string>{totalWordsPerFold},
  };

  double summedAccuracy = 0.0;
  auto numFolds = folds.size();

  for (const auto&[word, filePaths] : classificationPaths) {
    size_t index = 0;
    for (auto it = filePaths.begin(); it != filePaths.end(); ++it, ++index) {
      folds[index][word] = *it;
    }
  }

  // Training all examples
  for (const auto& currentTestingFold : folds) {
    for (const auto&[word, filePath] : currentTestingFold) {
      dictaWav.train(filePath, word);
    }
  }

  // K-Fold cross validation
  for (const auto& currentTestingFold : folds) {
    for (const auto&[word, filePath] : currentTestingFold) {
      dictaWav.forget(filePath, word);
    }

    size_t gotRight = 0;
    for (const auto&[word, filePath] : currentTestingFold) {
      auto classification = dictaWav.classify(filePath);
      if (word == classification)
        ++gotRight;
    }

    summedAccuracy += static_cast<double>(gotRight) / static_cast<double>(totalWordsPerFold);

    for (const auto&[word, filePath] : currentTestingFold) {
      dictaWav.train(filePath, word);
    }
  }

  double accuracy = summedAccuracy / static_cast<double>(numFolds);
  std::cout << "Got " << accuracy * 100.0 << "% of accuracy" << std::endl;

  return accuracy;
}