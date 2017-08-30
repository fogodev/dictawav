/*************************************************************\
|-------------------------------------------------------------|
|         Created by Ericson "Fogo" Soares on 13/03/17        |
|-------------------------------------------------------------|
|                 https://github.com/fogodev                  |
|-------------------------------------------------------------|
\*************************************************************/

#ifndef DICTAWAV_WAVHANDLER_H
#define DICTAWAV_WAVHANDLER_H

#include <string>
#include <stdexcept>
#include <vector>
#include <sndfile.hh>
#include <memory>

namespace DictaWav
{
  class WavHandler
  {
    private:
      SndfileHandle wavInfo;
      std::shared_ptr<std::vector<double>> audioDataPtr;
    
    public:
      WavHandler() = default;
      explicit WavHandler(std::string wavPath);
      
      void setWavInfo(std::string wavPath);
      
      std::size_t getSampleRate()
      { return this->wavInfo.samplerate(); }
      
      std::shared_ptr<std::vector<double>> getAudioData()
      { return this->audioDataPtr; }
    
    private:
      void extractAudioData();
      void convertToMono();
  };
}

#endif //DICTAWAV_WAVHANDLER_H
