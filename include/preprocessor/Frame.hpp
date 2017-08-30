/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_FRAME_H
#define DICTA_FRAME_H

#include <cstddef>
#include <stdexcept>

namespace DictaWav
{
  template <class T>
  class Frame
  {
    private:
      std::size_t numSamples = 0;
      std::size_t sampleCounter = 0;
      T* samples = nullptr;
    
    public:
      Frame() = default;
      
      explicit Frame(std::size_t numSamples) :
          numSamples(numSamples),
          samples(new T[numSamples])
      { }
      
      ~Frame()
      { delete[] this->samples; }
      
      // Array subscript operators
      T operator[](std::size_t index)
      { return this->samples[index]; }
      
      const T operator[](std::size_t index) const
      { return this->samples[index]; }
      
      // USE ONLY PUSH TO FILL THE CONTAINER
      void push(T sample)
      {
        if (this->sampleCounter < this->numSamples)
          this->samples[this->sampleCounter++] = sample;
        else
          throw std::out_of_range("Frame error: Cannot push a sample to a full frame");
      }
      
      std::size_t size()
      { return this->sampleCounter; }
      
      T* data() // WARNING: Pointer content will be deleted if Frame goes out of scope
      { return this->samples; }
      
      bool empty()
      { return this->sampleCounter == 0; }
      
      // Deleted copy constructor and operator
      Frame(const Frame& other) = delete;
      Frame& operator=(const Frame& other) = delete;
      
      // Move constructor
      Frame(Frame&& other) noexcept :
          numSamples(other.numSamples),
          sampleCounter(other.sampleCounter),
          samples(other.samples)
      {
        other.numSamples = 0;
        other.sampleCounter = 0;
        other.samples = nullptr;
      }
      
      // Move operator
      Frame& operator=(Frame&& other) noexcept
      {
        if (this != &other) {
          delete[] this->samples;
          
          this->numSamples = other.numSamples;
          this->sampleCounter = other.sampleCounter;
          this->samples = other.samples;
          
          other.numSamples = 0;
          other.sampleCounter = 0;
          other.samples = nullptr;
        }
        return *this;
      }
  };
}

#endif //DICTA_FRAME_H
