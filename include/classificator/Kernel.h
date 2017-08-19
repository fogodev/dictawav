/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_KERNEL_H
#define DICTA_KERNEL_H

#include <cstddef>
#include <cmath>
#include <random>

namespace DictaWav
{
  class Kernel
  {
    private:
      std::size_t dimension;
      double* coordinates;
    
    public:
      Kernel(std::size_t dimension) :
          dimension(dimension),
          coordinates(new double[dimension])
      {
        static std::default_random_engine randomEngine;
        static std::uniform_real_distribution<double> distribution(-1, 1);
        for (int coordinate = 0; coordinate != this->dimension; ++coordinate)
          this->coordinates[coordinate] = distribution(randomEngine);
      }
      
      Kernel(std::size_t dimension, std::vector<double> coordinates) :
          dimension(dimension),
          coordinates(new double[dimension])
      {
        for (std::size_t index = 0; index != coordinates.size(); ++index)
          this->coordinates[index] = coordinates[index];
      }
      
      ~Kernel()
      {
        delete[] this->coordinates;
      }
      
      // Deleted copy constructor and operator
      Kernel(const Kernel&) = delete;
      Kernel& operator=(const Kernel&) = delete;
      
      // Move constructor
      Kernel(Kernel&& other) noexcept :
          dimension(other.dimension),
          coordinates(other.coordinates)
      {
        other.dimension = 0;
        other.coordinates = nullptr;
      }
      
      // Move operator
      Kernel& operator=(Kernel&& other) noexcept
      {
        if (this != &other) {
          delete[] this->coordinates;
          
          this->dimension = other.dimension;
          this->coordinates = other.coordinates;
          
          other.dimension = 0;
          other.coordinates = nullptr;
        }
        return *this;
      }
      
      double checkDistance(const Kernel& other)
      {
        double distance = 0;
        for (auto coordinate = 0; coordinate != this->dimension; ++coordinate)
          distance += std::pow(this->coordinates[coordinate] - other.coordinates[coordinate], 2);
        
        return std::sqrt(distance);
      }
  };
}

#endif //DICTA_KERNEL_H
