/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 13/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#include "../../include/wav_handler/WavHandler.h"

namespace DictaWav
{
    WavHandler::WavHandler(std::string wavPath)
    {
        this->setWavInfo(wavPath);
    }
    
    void WavHandler::setWavInfo(std::string wavPath)
    {
        this->wavPath = wavPath;
        this->wavInfo = SndfileHandle(wavPath);
        
        if(this->wavInfo.error())
            throw new std::runtime_error("LibSndFile error: " + std::string(this->wavInfo.strError()));
    
        this->extractAudioData();
    }
    
    void WavHandler::extractAudioData()
    {
        sf_count_t readFrames;
        
        this->audioDataSize = this->wavInfo.frames() * this->wavInfo.channels();
        this->audioDataPtr = std::make_shared<std::vector<double>>(this->audioDataSize);
        readFrames = this->wavInfo.read(this->audioDataPtr->data(), this->audioDataSize);
        
        if(readFrames != this->audioDataSize)
            throw std::runtime_error("LibSndFile error: Couldn't read all the frames on wav file.");
        
        if(this->wavInfo.channels() > 1)
            this->convertToMono();
        
        if(this->audioDataPtr->empty())
            throw new std::runtime_error("LibSndFile error: Failed to read audio file.");
    }
    
    void WavHandler::convertToMono()
    {
        auto frames = this->wavInfo.frames();
        auto channelsCount = this->wavInfo.channels();
        
        std::vector<double> monoAudio(frames);
        for(int frame = 0; frame != frames; ++frame){
            for(int currentChannel = 0; currentChannel != channelsCount; ++currentChannel)
                monoAudio[frame] += this->audioDataPtr->operator[](frame * channelsCount + currentChannel);
            monoAudio[frame] /= channelsCount;
        }
        
        this->audioDataPtr = std::make_shared<decltype(monoAudio)>(monoAudio);
    }
}