/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_DISCRIMINATOR_H
#define DICTAWAV_DISCRIMINATOR_H

#include <vector>
#include <memory>
#include "Ram.h"

namespace DictaWav {
class Discriminator {
 private:
  size_t retinaSize;
  size_t ramNumBits;
  size_t ramsCount;
  std::vector<Ram> rams;
  std::shared_ptr<std::vector<size_t>> ramAddressMapping;

 public:
  Discriminator(
      size_t retinaSize,
      size_t ramNumBits,
      std::shared_ptr<std::vector<size_t>> ramAddressMapping,
      bool isCumulative = true
  ) :
      retinaSize(retinaSize),
      ramNumBits(ramNumBits),
      ramAddressMapping(ramAddressMapping),
      ramsCount(
          static_cast<size_t>(std::ceil(
              static_cast<double>(retinaSize) / static_cast<double>(ramNumBits)))
      ) {
    if (ramNumBits > 62)
      throw std::runtime_error(
          "WiSARD ERROR: Representation overflow due to number of bits being greater than 62."
      );

    this->rams.reserve(this->ramsCount);
    auto rest = retinaSize % ramNumBits;

    if (rest == 0) {
      for (size_t index = 0; index != this->ramsCount; ++index) {
        this->rams.emplace_back(Ram(ramNumBits, isCumulative));
      }
    } else {
      for (size_t index = 0; index != this->ramsCount - 1; ++index) {
        this->rams.emplace_back(Ram(ramNumBits, isCumulative));
      }
      // The remaining rams
      this->rams.emplace_back(Ram(rest, isCumulative));
    }
  }

  void train(const std::vector<char>& retina) {
    size_t address;
    size_t base;
    size_t ramIndex = 0;

    // Each group of ramNumBits is related with a ram
    for (size_t index = 0;
         index <= this->retinaSize - this->ramNumBits;
         index += this->ramNumBits) {
      address = 0;
      base = 1;

      for (size_t bitIndex = 0; bitIndex != this->ramNumBits; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[index + bitIndex]])
          address += base;
        base *= 2;
      }

      ramIndex = index / this->ramNumBits;
      this->rams[ramIndex].insert(address);
    }

    // The rest of the retina when retina's length isn't a multiple of bit's address number
    size_t restOfPositions = this->retinaSize % this->ramNumBits;
    if (restOfPositions != 0) {
      address = 0;
      base = 1;
      for (size_t bitIndex = 0; bitIndex != restOfPositions; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[this->retinaSize - restOfPositions - 1 + bitIndex]])
          address += base;
        base *= 2;
      }
      this->rams[ramIndex + 1].insert(address);
    }
  }
  void forget(const std::vector<char>& retina) {
    size_t address;
    size_t base;
    size_t ramIndex = 0;

    // Each group of ramNumBits is related with a ram
    for (size_t index = 0;
         index <= this->retinaSize - this->ramNumBits;
         index += this->ramNumBits) {
      address = 0;
      base = 1;

      for (size_t bitIndex = 0; bitIndex != this->ramNumBits; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[index + bitIndex]])
          address += base;
        base *= 2;
      }

      ramIndex = index / this->ramNumBits;
      this->rams[ramIndex].remove(address);
    }

    // The rest of the retina when retina's length isn't a multiple of bit's address number
    size_t restOfPositions = this->retinaSize % this->ramNumBits;
    if (restOfPositions != 0) {
      address = 0;
      base = 1;
      for (size_t bitIndex = 0; bitIndex != restOfPositions; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[this->retinaSize - restOfPositions - 1 + bitIndex]])
          address += base;
        base *= 2;
      }
      this->rams[ramIndex + 1].remove(address);
    }
  }
  std::vector<unsigned> classify(const std::vector<char>& retina) const {
    size_t ramIndex = 0;
    size_t address;
    size_t base;
    std::vector<unsigned> result;
    result.reserve(this->ramsCount);

    // Each group of ramNumBits is related with a ram
    for (size_t index = 0;
         index <= this->retinaSize - this->ramNumBits;
         index += this->ramNumBits) {
      address = 0;
      base = 1;

      for (size_t bitIndex = 0; bitIndex != this->ramNumBits; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[index + bitIndex]])
          address += base;
        base *= 2;
      }

      ramIndex = index / ramNumBits;
      result.push_back(this->rams[ramIndex].get(address));
    }

    // The rest of the retina, when retina's length isn't a multiple of bit's address number
    size_t restOfPositions = this->retinaSize % this->ramNumBits;
    if (restOfPositions != 0) {
      address = 0;
      base = 1;

      for (size_t bitIndex = 0; bitIndex != restOfPositions; ++bitIndex) {
        if (retina[(*(this->ramAddressMapping))[this->retinaSize - restOfPositions - 1 + bitIndex]])
          address += base;
        base *= 2;
      }
      result.push_back(this->rams[ramIndex + 1].get(address));
    }

    return std::move(result);
  }
};

}

#endif //DICTAWAV_DISCRIMINATOR_H
