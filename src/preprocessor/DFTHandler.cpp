/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 07/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/preprocessor/DFTHandler.h"

namespace DictaWav
{
  // Float version constructor
  DFTHandler::DFTHandler(std::size_t fftSize, std::size_t dctSize, float) :
      fftSize(fftSize),
      dctSize(dctSize),
      outputSize(fftSize / 2 + 1),
      fftFloatInput(fftwf_alloc_real(fftSize)),
      fftFloatOutput(fftwf_alloc_complex(fftSize / 2 + 1)),
      dctFloatInput(fftwf_alloc_real(dctSize)),
      dctFloatOutput(fftwf_alloc_real(dctSize))
  {
    fftw_import_wisdom_from_filename(this->wisdomFloatFileName.c_str());
    
    this->fftFloatPlan = fftwf_plan_dft_r2c_1d(
        fftSize,
        fftFloatInput,
        fftFloatOutput,
        FFTW_PATIENT | FFTW_DESTROY_INPUT);
    this->dctFloatPlan = fftwf_plan_r2r_1d(
        dctSize,
        dctFloatInput,
        dctFloatOutput,
        FFTW_REDFT10,
        FFTW_PATIENT | FFTW_DESTROY_INPUT);
    
    if (fftFloatPlan == NULL || dctFloatPlan == NULL)
      throw std::runtime_error("FFTW3 error: Couldn't make plans for FFT or DCT");
    
    if (!fftw_export_wisdom_to_filename(this->wisdomFloatFileName.c_str()))
      throw std::runtime_error("FFTW3 error: Couldn't save wisdom to file");
  }
  
  // Double version constructor
  DFTHandler::DFTHandler(std::size_t fftSize, std::size_t dctSize, double) :
      fftSize(fftSize),
      dctSize(dctSize),
      outputSize(fftSize / 2 + 1),
      fftDoubleInput(fftw_alloc_real(fftSize)),
      fftDoubleOutput(fftw_alloc_complex(fftSize / 2 + 1)),
      dctDoubleInput(fftw_alloc_real(dctSize)),
      dctDoubleOutput(fftw_alloc_real(dctSize))
  {
    fftw_import_wisdom_from_filename(this->wisdomDoubleFileName.c_str());
    
    this->fftDoublePlan = fftw_plan_dft_r2c_1d(
        fftSize,
        fftDoubleInput,
        fftDoubleOutput,
        FFTW_PATIENT | FFTW_DESTROY_INPUT);
    this->dctDoublePlan = fftw_plan_r2r_1d(
        dctSize,
        dctDoubleInput,
        dctDoubleOutput,
        FFTW_REDFT10,
        FFTW_PATIENT | FFTW_DESTROY_INPUT);
    
    if (fftDoublePlan == NULL || dctDoublePlan == NULL)
      throw std::runtime_error("FFTW3 error: Couldn't make plans for FFT or DCT");
    
    if (!fftw_export_wisdom_to_filename(this->wisdomDoubleFileName.c_str()))
      throw std::runtime_error("FFTW3 error: Couldn't save wisdom to file");
  }
  
  DFTHandler::~DFTHandler()
  {
    if (!this->fftFloatInput) fftwf_free(this->fftFloatInput);
    if (!this->fftFloatOutput) fftwf_free(this->fftFloatOutput);
    if (!this->dctFloatInput) fftwf_free(this->dctFloatInput);
    if (!this->dctFloatOutput) fftwf_free(this->dctFloatOutput);
    if (!this->fftFloatPlan) fftwf_destroy_plan(this->fftFloatPlan);
    if (!this->dctFloatPlan) fftwf_destroy_plan(this->dctFloatPlan);
    
    if (!this->fftDoubleInput) fftw_free(this->fftDoubleInput);
    if (!this->fftDoubleOutput) fftw_free(this->fftDoubleOutput);
    if (!this->dctDoubleInput) fftw_free(this->dctDoubleInput);
    if (!this->dctDoubleOutput) fftw_free(this->dctDoubleOutput);
    if (!this->fftDoublePlan) fftw_destroy_plan(this->fftDoublePlan);
    if (!this->dctDoublePlan) fftw_destroy_plan(this->dctDoublePlan);
  }
  
  // Float version FFT
  Frame<float> DFTHandler::processFFT(const Frame<float>& input)
  {
    for (auto pos = 0; pos != this->fftSize; ++pos)
      this->fftFloatInput[pos] = input[pos];
    
    fftwf_execute(this->fftFloatPlan);
    
    Frame<float> frame(this->outputSize);
    float real = 0;
    float imaginary = 0;
    
    for (auto pos = 0; pos != this->outputSize; ++pos) {
      real = this->fftFloatOutput[pos][0];
      imaginary = this->fftFloatOutput[pos][1];
      frame.push(std::sqrt((real * real) + (imaginary * imaginary)));
    }
    
    return frame;
  }
  
  // Float version DCT
  Frame<float> DFTHandler::processDCT(const Frame<float>& input)
  {
    for (auto pos = 0; pos != this->dctSize; ++pos)
      this->dctFloatInput[pos] = input[pos];
    
    fftwf_execute(this->dctFloatPlan);
    
    Frame<float> frame(this->dctSize / 2);
    for (auto pos = 0; pos != this->dctSize / 2; ++pos)
      frame.push(std::tanh(this->dctFloatOutput[pos])); // Normalizing between -1 and 1
    
    return frame;
  }
  
  // Double version FFT
  Frame<double> DFTHandler::processFFT(const Frame<double>& input)
  {
    for (auto pos = 0; pos != this->fftSize; ++pos)
      this->fftDoubleInput[pos] = input[pos];
    
    fftw_execute(this->fftDoublePlan);
    
    Frame<double> frame(this->outputSize);
    double real = 0;
    double imaginary = 0;
    
    for (auto pos = 0; pos != this->outputSize; ++pos) {
      real = this->fftDoubleOutput[pos][0];
      imaginary = this->fftDoubleOutput[pos][1];
      frame.push(std::sqrt((real * real) + (imaginary * imaginary)));
    }
    return frame;
  }
  
  // Double version DCT
  Frame<double> DFTHandler::processDCT(const Frame<double>& input)
  {
    for (auto pos = 0; pos != this->dctSize; ++pos)
      this->dctDoubleInput[pos] = input[pos];
    
    fftw_execute(this->dctDoublePlan);
    
    Frame<double> frame(this->dctSize / 2);
    for (auto pos = 0; pos != this->dctSize / 2; ++pos)
      frame.push(std::tanh(this->dctDoubleOutput[pos]));  // Normalizing between -1 and 1
    
    return frame;
  }
}