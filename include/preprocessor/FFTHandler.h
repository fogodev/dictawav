/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 07/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_FFTHANDLER_H
#define DICTAWAV_FFTHANDLER_H

#include <stdexcept>
#include <vector>
#include <cmath>
#include <fftw3.h>

namespace DictaWav {

class FFTHandler {
  fftw_plan fft;
  size_t size;
  fftw_complex* input;
  fftw_complex* output;

  const char* wisdomFileName = "./fftWisdomFile.data";

 public:
  FFTHandler(size_t size) : size(size),
                            input(fftw_alloc_complex(size)),
                            output(fftw_alloc_complex(size)) {
    fftw_import_wisdom_from_filename(this->wisdomFileName);

    this->fft = fftw_plan_dft_1d(
        size,
        input,
        output,
        FFTW_FORWARD,
        FFTW_PATIENT | FFTW_DESTROY_INPUT
    );

    if (this->fft == NULL) {
      throw std::runtime_error("FFTW3 error: Couldn't make plans for FFT");
    }

    if (!fftw_export_wisdom_to_filename(this->wisdomFileName)) {
      throw std::runtime_error("FFTW3 error: Couldn't save wisdom to file");
    }

    for (size_t index = 0; index != size; ++index) {
      this->input[index][0] = 0.0;
      this->input[index][1] = 0.0;
      this->output[index][0] = 0.0;
      this->output[index][1] = 0.0;
    }
  }

  ~FFTHandler() {
    fftw_free(this->input);
    fftw_free(this->output);
    fftw_destroy_plan(this->fft);
  }

  std::vector<double> process(const std::vector<double>& input) {
    for (auto pos = 0; pos != this->size; ++pos) {
      this->input[pos][0] = input[pos];
      this->input[pos][1] = 0.0;
    }

    fftw_execute(this->fft);

    std::vector<double> frame;
    frame.reserve(this->size);

    for (auto pos = 0; pos != this->size; ++pos) {
      double real = this->output[pos][0];
      double imaginary = this->output[pos][1];
      this->output[pos][0] = 0.0;
      this->output[pos][1] = 0.0;
      frame.push_back(std::sqrt((real * real) + (imaginary * imaginary)));
    }
    return std::move(frame);
  }
};

}

#endif //DICTAWAV_FFTHANDLER_H
