/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_RAM_H
#define DICTAWAV_RAM_H

#include <unordered_map>
#include <exception>
#include <cmath>

namespace DictaWav {

class Ram {
 private:
  std::unordered_map<size_t, unsigned> data;
  size_t maxAddress;
  bool isCumulative;

 public:
  explicit Ram(size_t numBits, bool isCumulative = true) :
      isCumulative(isCumulative),
      maxAddress(static_cast<size_t>(std::pow(static_cast<size_t>(2), numBits))) {}

  void insert(size_t address) {
    if (address > this->maxAddress)
      throw std::runtime_error(
          "WiSARD-RAM ERROR: Pushing address out of range 0 to "
              + std::to_string(this->maxAddress)
      );

    if (!this->isCumulative)
      this->data[address] = 1;
    else if (this->data.find(address) == this->data.end())
      this->data[address] = 1;
    else
      this->data[address] += 1;
  }

  void remove(size_t address) {
    if (address > this->maxAddress)
      throw std::runtime_error(
          "WiSARD-RAM ERROR: Removing address out of range 0 to " + std::to_string(this->maxAddress)
      );

    if (!this->isCumulative)
      this->data[address] = 0;
    else if (this->data.find(address) == this->data.end())
      this->data[address] = 0;
    else if (this->data[address] > 0)
      this->data[address] -= 1;
  }

  unsigned get(size_t address) const {
    if (this->data.find(address) == this->data.end())
      return 0;

    return this->data.at(address);
  }
};

}

#endif //DICTAWAV_RAM_H
