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
      std::unordered_map<std::size_t, unsigned> data{};
      std::size_t maxAddress;
      std::size_t bitsCount;
      bool isCumulative;
    
    public:
      explicit Ram(std::size_t bitsCount, bool isCumulative = true);
      ~Ram();
      
      void push(const std::size_t& address);
      unsigned getValue(const std::size_t& address);
  };
}

#endif //DICTAWAV_RAM_H
