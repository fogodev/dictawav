/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_RAM_H
#define DICTAWAV_RAM_H

#include <cstddef>
#include <unordered_map>
#include <exception>
#include <cmath>

namespace DictaWav
{
    class Ram
    {
        private:
        std::unordered_map<long long, int> data;
        unsigned long long addressCount;
        std::size_t bitsCount;
        
        public:
        Ram(std::size_t bitsCount);
        
        void push(const long long address, int value);
        int getValue(const long long address);
    };
}

#endif //DICTAWAV_RAM_H
