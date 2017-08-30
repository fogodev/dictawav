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
      Discriminator(
          std::size_t retinaSize,
          std::size_t numBitsAddress,
          std::vector<std::size_t> ramAddressMapping,
          bool isCumulative = true
                   );
      
      ~Discriminator();
      
      void train(std::vector<char> retina);
      std::vector<unsigned> classify(std::vector<char> retina);
  };
}

#endif //DICTAWAV_DISCRIMINATOR_H
