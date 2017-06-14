/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/


#include "../../include/wisard/Wisard.h"

namespace DictaWav
{
    Wisard::Wisard(std::size_t retinaSize, std::size_t numBitsAddr, double confidenceMinimunRate) :
        retinaSize(retinaSize),
        numBitsAddress(numBitsAddr),
        confidenceMinimumRate(confidenceMinimunRate)
    {
        for(std::size_t index; index != retinaSize; ++index)
            this->ramAddressMapping.push_back(index);
        
        std::shuffle(
                this->ramAddressMapping.begin(),
                this->ramAddressMapping.end(),
                std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
        );
    }
    
    Wisard::~Wisard()
    {
        for(auto iterator = this->discriminators.begin(); iterator != this->discriminators.end(); ++iterator)
            delete iterator->second;
    }
    
    void Wisard::train(const std::vector<std::vector<int>>& trainingRetinas, const std::vector<std::string>& classNames)
    {
        // Getting unique labels
        std::vector<std::string> labels;
        std::unordered_map<std::string, int> auxiliaryMap;
        
        for(std::size_t index = 0; index != classNames.size(); ++index)
            auxiliaryMap[classNames[index]] = 0;
        
        for(auto iterator = auxiliaryMap.begin(); iterator != auxiliaryMap.end(); ++iterator)
            labels.push_back(iterator->first);
        
        // Creating discriminators
        for(size_t index = 0; index != labels.size(); ++index){
            Discriminator *discriminator = new Discriminator(this->retinaSize,
                                                             this->numBitsAddress,
                                                             this->ramAddressMapping
            );
            this->discriminators[labels[index]] = discriminator;
        }
        
        // Training discriminators
        for(std::size_t index = 0; index != classNames.size(); ++index)
            this->discriminators[classNames[index]]->train(trainingRetinas[index]);
    }
    
    std::vector<std::unordered_map<std::string, double>> Wisard::classificationAndProbability(const std::vector<std::vector<int>>& retinas)
    {
        std::vector<std::unordered_map<std::string, double>> results;
        int ramsCount = static_cast<int>(
                std::ceil(
                        static_cast<double>(this->retinaSize) / static_cast<double>(this->numBitsAddress)
                )
        );
        
        // For each retina
        for(std::size_t index = 0; index != retinas.size(); ++index){
            std::string label;
            
            std::unordered_map<std::string, double> result;
            std::unordered_map<std::string, std::vector<int>> ramResult;
            
            Discriminator* auxiliaryDiscriminator;
            std::vector<int> auxiliaryRamResult;
            
            // Get retina
            auto retina = retinas[index];
            
            // Test with all discriminators
            for(auto iterator = this->discriminators.begin(); iterator != this->discriminators.end(); ++iterator){
                label = iterator->first;
                auxiliaryDiscriminator = iterator->second;
                
                auxiliaryRamResult = auxiliaryDiscriminator->classify(retina);
                
                int sumRamsValue = 0;
                for(std::size_t ramResultsIndex = 0; ramResultsIndex != auxiliaryRamResult.size(); ++ramResultsIndex)
                    if(auxiliaryRamResult[ramResultsIndex] > 0)
                        ++sumRamsValue;
    
                // Calculating probability
                result[label] = static_cast<double>(sumRamsValue) / static_cast<double>(ramsCount);
                ramResult[label] = auxiliaryRamResult;
            }
            
            results.push_back(std::move(result));
        }
        
        return results;
    }
    
    std::vector<std::string> Wisard::classify(const std::vector<std::vector<int>>& retinas)
    {
        std::vector<std::string> resultLabels;
        auto results = this->classificationAndProbability(retinas);
        
        for(std::size_t index = 0; index != results.size(); ++index){
            auto result = results[index];
            double max = 0;
            std::string maxLabel{};
            
            for(auto iterator = result.begin(); iterator != result.end(); ++iterator){
                auto currentValue = iterator->second;
                if(max <= currentValue){
                    max = currentValue;
                    maxLabel = iterator->first;
                }
            }
            
            resultLabels.push_back(maxLabel);
        }
        
        return resultLabels;
    }
}