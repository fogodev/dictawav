/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/classificator/Discriminator.h"

namespace DictaWav
{
  Discriminator::Discriminator(
      std::size_t retinaSize,
      std::size_t numBitsAddress,
      std::vector<std::size_t> ramAddressMapping,
      bool isCumulative
                              ) :
      retinaSize(retinaSize),
      numBitsAddress(numBitsAddress),
      ramAddressMapping(ramAddressMapping),
      ramsCount(
          static_cast<std::size_t>(std::ceil(
              static_cast<double>(retinaSize) / static_cast<double>(numBitsAddress))))
  {
    if (retinaSize % numBitsAddress == 0)
      for (std::size_t index = 0; index != this->ramsCount; ++index)
        this->rams.push_back(new Ram(numBitsAddress, isCumulative));
    else {
      for (std::size_t index = 0; index != this->ramsCount - 1; ++index)
        this->rams.push_back(new Ram(numBitsAddress, isCumulative));
      
      // The remaining rams
      this->rams.push_back(new Ram(retinaSize % numBitsAddress, isCumulative));
    }
  }
  
  Discriminator::~Discriminator()
  {
    for (std::size_t index = 0; index != this->rams.size(); ++index)
      delete this->rams[index];
  }
  
  void Discriminator::train(std::vector<char> trainingRetina)
  {
    std::size_t ramIndex = 0;
    std::size_t address = 0;
    std::size_t restOfPositions = 0;
    std::size_t base;
    
    // Each group of numBitsAddress is related with a ram
    for (std::size_t index = 0;
         index <= this->retinaSize - this->numBitsAddress;
         index += this->numBitsAddress) {
      address = 0;
      base = 1;
      
      for (std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex) {
        if (trainingRetina[this->ramAddressMapping[index + bitIndex]])
          address += base;
        base *= 2;
      }
      
      ramIndex = index / this->numBitsAddress;
      this->rams[ramIndex]->push(address);
    }
    
    // The rest of the retina when retina's length isn't a multiple of bit's address number
    restOfPositions = this->retinaSize % this->numBitsAddress;
    if (!restOfPositions) {
      address = 0;
      base = 1;
      for (std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex) {
        if (trainingRetina[this->ramAddressMapping[this->retinaSize - restOfPositions - 1 + bitIndex]])
          address += base;
        base *= 2;
      }
      this->rams[ramIndex + 1]->push(address);
    }
  }
  
  std::vector<unsigned> Discriminator::classify(std::vector<char> retina)
  {
    std::size_t ramIndex = 0;
    std::size_t address = 0;
    std::size_t restOfPositions = 0;
    std::size_t base;
    std::vector<unsigned> result(this->ramsCount);
    
    // Each group of numBitsAddress is related with a ram
    for (std::size_t index = 0;
         index <= this->retinaSize - this->numBitsAddress;
         index += this->numBitsAddress) {
      address = 0;
      base = 1;
      
      for (std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex) {
        if (retina[this->ramAddressMapping[index + bitIndex]])
          address += base;
        base *= 2;
      }
      ramIndex = index / numBitsAddress;
      result.push_back(this->rams[ramIndex]->getValue(address));
    }
    
    // The rest of the retina, when retina's length isn't a multiple of bit's address number
    restOfPositions = this->retinaSize % this->numBitsAddress;
    if (!restOfPositions) {
      address = 0;
      base = 1;
      for (std::size_t bitIndex = 0; bitIndex != restOfPositions; ++bitIndex) {
        if (retina[this->ramAddressMapping[this->retinaSize - restOfPositions - 1 + bitIndex]])
          address += base;
        base *= 2;
      }
      result.push_back(this->rams[ramIndex + 1]->getValue(address));
    }
    
    return std::move(result);
  }
}