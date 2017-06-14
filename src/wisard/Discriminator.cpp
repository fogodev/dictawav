/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/wisard/Discriminator.h"

namespace DictaWav
{
    Discriminator::Discriminator(std::size_t retinaSize, std::size_t numBitsAdress, std::vector<std::size_t> ramAdressMapping) :
            retinaSize(retinaSize),
            numBitsAddress(numBitsAdress),
            ramAddressMapping(ramAdressMapping),
            ramsCount(static_cast<std::size_t>(std::ceil(static_cast<double>(retinaSize) / static_cast<double>(numBitsAdress))))
    {
        if(!(retinaSize % numBitsAdress > 0))
            for(std::size_t index = 0; index != this->ramsCount; ++index)
                this->rams.push_back(new Ram(numBitsAdress));
        else{
            for(std::size_t index = 0; index != this->ramsCount - 1; ++index)
                this->rams.push_back(new Ram(numBitsAdress));
            
            // The rest
            this->rams.push_back(new Ram(retinaSize % numBitsAdress));
        }
    }
    
    Discriminator::~Discriminator()
    {
        for(std::size_t index = 0; index != this->rams.size(); ++index)
            delete this->rams[index];
    }
    
    void Discriminator::train(const std::vector<int>& retina)
    {
        int ramIndex;
        long long address;
        int restOfPositions;
        long long base;
        
        // Each group of numBitsAddress is related with a ram
        for(std::size_t index = 0; index <= this->retinaSize - this->numBitsAddress; index += this->numBitsAddress){
            address = 0LL;
            base = 1LL;
            
            for(std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex){
                if(retina[this->ramAddressMapping[index + bitIndex]] != 0)
                    address += base;
                base *= 2LL;
            }
            
            ramIndex = static_cast<int>(index / this->numBitsAddress);
            this->rams[ramIndex]->push(address, 1);
        }
        
        // The rest of the retina (when retina's length isn't a multiple of bit's address number
        restOfPositions = this->retinaSize % this->numBitsAddress;
        if(!restOfPositions){
            address = 0LL;
            base = 1LL;
            for(std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex){
                if(retina[this->ramAddressMapping[this->retinaSize - restOfPositions - 1 + bitIndex]] != 0)
                    address += base;
                base *= 2LL;
            }
            this->rams[ramIndex + 1]->push(address, 1);
        }
    }
    
    std::vector<int> Discriminator::classify(const std::vector<int>& retina)
    {
        long long address;
        int ramIndex;
        int restOfPositions;
        long long base;
        std::vector<int> result(this->ramsCount);
        
        // Each group of numBitsAddress is related with a ram
        for(std::size_t index = 0; index <= this->retinaSize - this->numBitsAddress; index += this->numBitsAddress){
            address = 0LL;
            base = 1LL;
            
            for(std::size_t bitIndex = 0; bitIndex != this->numBitsAddress; ++bitIndex){
                if(retina[this->ramAddressMapping[index + bitIndex]] != 0)
                    address += base;
                base *= 2LL;
            }
            ramIndex = static_cast<int>(index / numBitsAddress);
            result.push_back(this->rams[ramIndex]->getValue(address));
        }
        
        // The rest of the retina, when retina's length isn't a multiple of bit's address number
        restOfPositions = this->retinaSize % this->numBitsAddress;
        if(!restOfPositions){
            address = 0LL;
            base = 1LL;
            for(std::size_t bitIndex = 0; bitIndex != restOfPositions; ++bitIndex){
                if(retina[this->ramAddressMapping[this->retinaSize - restOfPositions - 1 + bitIndex]] != 0)
                    address += base;
                base *= 2LL;
            }
            result.push_back(this->rams[ramIndex + 1]->getValue(address));
        }
        
        return result;
    }
}