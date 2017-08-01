/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/


#include "../../include/wisard/Ram.h"

namespace DictaWav
{
  Ram::Ram(std::size_t bitsCount) :
      bitsCount(bitsCount)
  {
    if (bitsCount > 62)
      throw std::runtime_error(
          "ERROR: Representation overflow due to number of bits being greater than 62."
      );
    this->addressCount = static_cast<unsigned long long>(std::pow(
        static_cast<long long>(2),
        static_cast<long long>(bitsCount))
    );
  }
  
  void Ram::push(const long long address, int value)
  {
    if (address < 0L || address >= this->addressCount)
      throw std::runtime_error(
          "ERROR: Pushing address out of range 0 to " + std::to_string(this->addressCount));
    
    if (!this->data.count(address))
      this->data[address] = value;
    else
      this->data[address] += value;
  }
  
  int Ram::getValue(const long long address)
  {
    if (address < 0L || address >= this->addressCount)
      throw std::runtime_error(
          "ERROR: Pushing address out of range 0 to " + std::to_string(this->addressCount));
    
    if (!this->data.count(address))
      return 0;
    
    return this->data[address];
  }
}