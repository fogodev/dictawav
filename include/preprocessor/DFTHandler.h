/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 07/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_DFTHANDLER_H
#define DICTA_DFTHANDLER_H

#include <cstddef>
#include <cmath>
#include <fftw3.h>
#include "Frame.hpp"

namespace DictaWav
{
    class DFTHandler
    {
        private:
        std::size_t fftSize;
        std::size_t dctSize;
        std::size_t outputSize;
        
        // Float version
        float* fftFloatInput;
        fftwf_complex* fftFloatOutput;
        float* dctFloatInput;
        float* dctFloatOutput;
        fftwf_plan fftFloatPlan;
        fftwf_plan dctFloatPlan;
        
        // Double version
        double* fftDoubleInput;
        fftw_complex* fftDoubleOutput;
        double* dctDoubleInput;
        double* dctDoubleOutput;
        fftw_plan fftDoublePlan;
        fftw_plan dctDoublePlan;
        
        const std::string wisdomFloatFileName = "./fftWisdomFloatFile.data";
        const std::string wisdomDoubleFileName = "./fftWisdomDoubleFile.data";
        
        public:
        DFTHandler(std::size_t fftSize, std::size_t dctSize, float);
        DFTHandler(std::size_t fftSize, std::size_t dctSize, double);
        ~DFTHandler();
        
        Frame<float> processFFT(const Frame<float>& input);
        Frame<float> processDCT(const Frame<float>& input);
        
        Frame<double> processFFT(const Frame<double>& input);
        Frame<double> processDCT(const Frame<double>& input);
    };
}

#endif //DICTA_DFTHANDLER_H
