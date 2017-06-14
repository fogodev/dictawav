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
#include "Discriminator.h"

namespace DictaWav
{
    class Wisard
    {
        private:
        std::size_t retinaSize;
        std::size_t numBitsAddress;
        double confidenceMinimumRate;
        std::unordered_map<std::string, Discriminator*> discriminators;
        std::vector<std::size_t> ramAddressMapping;
        
        public:
        Wisard(std::size_t retinaSize, std::size_t numBitsAddr, double confidenceMinimunRate = 0.1);
        ~Wisard();
        
        void train(const std::vector<std::vector<int>>& trainingRetinas, const std::vector<std::string>& classNames);
        std::vector<std::string> classify(const std::vector<std::vector<int>>& retinas);
        std::vector<std::unordered_map<std::string, double>> classificationAndProbability(const std::vector<std::vector<int>>& retinas);
    };
}

#endif //DICTAWAV_WISARD_H
