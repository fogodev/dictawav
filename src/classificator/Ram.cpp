/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/04/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/


#include "../../include/classificator/Ram.h"

namespace DictaWav
{
  Ram::Ram(std::size_t bitsCount, bool isCumulative = true) :
      bitsCount(bitsCount),
      isCumulative(isCumulative)
  {
    if (bitsCount > 62)
      throw std::runtime_error(
          "ERROR: Representation overflow due to number of bits being greater than 62."
      );
    this->maxAddress = static_cast<std::size_t>(std::pow(static_cast<std::size_t>(2), bitsCount));
  }
  
  Ram::~Ram()
  {
    this->data.clear();
  }
  
  void Ram::push(const std::size_t& address)
  {
    if (address >= this->maxAddress)
      throw std::runtime_error(
          "ERROR: Pushing address out of range 0 to " + std::to_string(this->maxAddress));
    
    if (!this->isCumulative)
      this->data[address] = 1;
    else if (this->data.find(address) == this->data.end())
      this->data[address] = 1;
    else
      ++this->data[address];
  }
  
  unsigned Ram::getValue(const std::size_t& address)
  {
    if (address >= this->maxAddress)
      throw std::runtime_error(
          "ERROR: Pushing address out of range 0 to " + std::to_string(this->maxAddress));
    
    if (this->data.find(address) == this->data.end())
      return 0;
    
    return this->data[address];
  }
}