/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 07/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_DCTHANDLER_H
#define DICTAWAV_DCTHANDLER_H

#include <stdexcept>
#include <vector>
#include <cmath>
#include <fftw3.h>

namespace DictaWav {

class DCTHandler {
  fftw_plan dct;
  size_t size;
  double* input;
  double* output;

  const char* wisdomFileName = "./fftWisdomFile.data";

 public:
  DCTHandler(size_t size) :
      size(size),
      input(fftw_alloc_real(size)),
      output(fftw_alloc_real(size)) {
    fftw_import_wisdom_from_filename(this->wisdomFileName);

    this->dct = fftw_plan_r2r_1d(
        size,
        input,
        output,
        FFTW_REDFT10,
        FFTW_PATIENT | FFTW_DESTROY_INPUT
    );

    if (this->dct == NULL) {
      throw std::runtime_error("FFTW3 error: Couldn't make plans for DCT");
    }

    if (!fftw_export_wisdom_to_filename(this->wisdomFileName)) {
      throw std::runtime_error("FFTW3 error: Couldn't save wisdom to file");
    }

    for (size_t index = 0; index != size; ++index) {
      this->input[index] = 0.0;
      this->output[index] = 0.0;
    }
  }

  ~DCTHandler() {
    fftw_free(this->input);
    fftw_free(this->output);
    fftw_destroy_plan(this->dct);
  }

  std::vector<double> process(const std::vector<double>& input) {
    for (auto pos = 0; pos != this->size; ++pos)
      this->input[pos] = input[pos];

    fftw_execute(this->dct);

    std::vector<double> frame;
    frame.reserve(this->size / 2);
    for (auto pos = 0; pos != this->size / 2; ++pos) {
      frame.push_back(this->output[pos]);
      this->output[pos] = 0.0;
    }

    return std::move(frame);
  }
};

}

#endif //DICTAWAV_DCTHANDLER_H
