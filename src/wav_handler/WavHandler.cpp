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
    this->wavInfo = SndfileHandle(wavPath);
    
    if (this->wavInfo.error())
      throw std::runtime_error("LibSndFile error: " + std::string(this->wavInfo.strError()));
    
    this->extractAudioData();
  }
  
  void WavHandler::extractAudioData()
  {
    sf_count_t readFrames;
    sf_count_t audioDataSize = this->wavInfo.frames() * this->wavInfo.channels();
    
    this->audioDataPtr = std::make_shared<std::vector<double>>(audioDataSize);
    readFrames = this->wavInfo.read(this->audioDataPtr->data(), audioDataSize);
    
    if (readFrames != audioDataSize)
      throw std::runtime_error("LibSndFile error: Couldn't read all the frames on wav file.");
    
    if (this->wavInfo.channels() > 1)
      this->convertToMono();
    
    if (this->audioDataPtr->empty())
      throw std::runtime_error("LibSndFile error: Failed to read audio file.");
  }
  
  void WavHandler::convertToMono()
  {
    auto frames = this->wavInfo.frames();
    auto channelsCount = this->wavInfo.channels();
    
    std::vector<double>* monoAudio = new std::vector<double>(frames);
    for (std::size_t frame = 0; frame != frames; ++frame) {
      for (std::size_t currentChannel = 0; currentChannel != channelsCount; ++currentChannel)
        (*monoAudio)[frame] += (*this->audioDataPtr)[frame * channelsCount + currentChannel];
      (*monoAudio)[frame] /= channelsCount;
    }
    
    this->audioDataPtr = std::shared_ptr<std::vector<double>>(monoAudio);
  }
}