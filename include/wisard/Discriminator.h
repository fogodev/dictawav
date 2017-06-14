/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_DISCRIMINATOR_H
#define DICTAWAV_DISCRIMINATOR_H

#include <cstddef>
#include <vector>
#include "Ram.h"

namespace DictaWav
{
    class Discriminator
    {
        private:
        std::size_t retinaSize;
        std::size_t numBitsAddress;
        std::size_t ramsCount;
        std::vector<Ram*> rams;
        std::vector<std::size_t> ramAddressMapping;
        
        public:
        Discriminator(std::size_t retinaSize, std::size_t numBitsAdress, std::vector<std::size_t> ramAdressMapping);
        ~Discriminator();
        
        void train(const std::vector<int>& retina);
        std::vector<int> classify(const std::vector<int>& retina);
    };
}

#endif //DICTAWAV_DISCRIMINATOR_H
