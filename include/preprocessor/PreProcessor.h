/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 06/02/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTA_PREPROCESSOR_H
#define DICTA_PREPROCESSOR_H

#include <queue>
#include <boost/circular_buffer.hpp>
#include <cmath>
#include <iostream>
#include "Frame.hpp"
#include "DFTHandler.h"
#include "MFCC.hpp"

namespace DictaWav
{
    class PreProcessor
    {
        public:
        using FrameType = Frame<double>;
        
        private:
        std::size_t sampleRate;
        std::size_t samplesPerFrame;
        std::vector<FrameType> processedFrames;
        DFTHandler dftHandler;
        MFCC<double> mfcc;
        
        static constexpr std::size_t filterBankCount = 26;
        static constexpr std::size_t lowerFrequency = 0;
        static constexpr float dftFloat = float{};
        static constexpr double dftDouble = double{};
        static constexpr double pi = std::atan(1) * 4;
        
        public:
        PreProcessor(std::size_t sampleRate) :
                sampleRate(sampleRate),
                samplesPerFrame(getNextPowerOf2(sampleRate / 100)), // To get 10ms sized Frames
                dftHandler(samplesPerFrame, filterBankCount, dftDouble),
                mfcc(sampleRate, filterBankCount, samplesPerFrame, lowerFrequency, calculateHigherFrequency(sampleRate))
        {}
        
        void addFrame(Frame<double> frame)
        { this->processedFrames.push_back(std::move(frame)); }
        
        double hannWindowFunction(std::size_t index)
        { return 0.5 * (1 - std::cos((2 * pi * index) / this->samplesPerFrame)); }
        
        std::size_t calculateHigherFrequency(std::size_t sampleRate)
        { return sampleRate / 2; }
        
        void process(std::shared_ptr<std::vector<double>> audioData);
        
        std::vector<Frame<double>>&& getFrames()
        { return std::move(this->processedFrames); }
        
        // For debug purpose only
        void report();
        
        private:
        std::size_t getNextPowerOf2(std::size_t num)
        {
            std::size_t base2 = 1;
            while(base2 < num)
                base2 <<= 1;
            return base2;
        }
    };
}
#endif //DICTA_PREPROCESSOR_H
