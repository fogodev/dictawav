/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 03/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_KERNEL_H
#define DICTA_KERNEL_H

#include <cmath>
#include <random>
#include <limits>

namespace DictaWav {

class Kernel {
 private:
  size_t dimension;
  double* coordinates;

 public:
  explicit Kernel(size_t dimension) :
      dimension(dimension),
      coordinates(new double[dimension]) {
    static std::random_device random;
    static std::default_random_engine randomEngine(random());
    static std::uniform_real_distribution<double>
        distribution(-1.0, 1.0 + std::numeric_limits<double>::min());
    for (size_t coordinate = 0; coordinate != this->dimension; ++coordinate)
      this->coordinates[coordinate] = distribution(randomEngine);
  }

  Kernel(size_t dimension, std::vector<double> coordinates) :
      dimension(dimension),
      coordinates(new double[dimension]) {
    for (size_t index = 0; index != coordinates.size(); ++index)
      this->coordinates[index] = coordinates[index];
  }

  ~Kernel() {
    delete[] this->coordinates;
  }

  // Deleted copy constructor and operator
  Kernel(const Kernel&) = delete;
  Kernel& operator=(const Kernel&) = delete;

  // Move constructor
  Kernel(Kernel&& other) noexcept :
      dimension(other.dimension),
      coordinates(other.coordinates) {
    other.dimension = 0;
    other.coordinates = nullptr;
  }

  // Move operator
  Kernel& operator=(Kernel&& other) noexcept {
    if (this != &other) {
      delete[] this->coordinates;

      this->dimension = other.dimension;
      this->coordinates = other.coordinates;

      other.dimension = 0;
      other.coordinates = nullptr;
    }
    return *this;
  }

  double checkDistanceSquared(const Kernel& other) {
    double distance = 0.0;
    for (size_t coordinate = 0; coordinate != this->dimension; ++coordinate) {
      double current = this->coordinates[coordinate] - other.coordinates[coordinate];
      distance += current * current;
    }

    return distance;
  }
};

}

#endif //DICTA_KERNEL_H
