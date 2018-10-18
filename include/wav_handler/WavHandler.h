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

namespace DictaWav {

class WavHandler {
 private:
  SndfileHandle wavInfo;
  std::vector<double> audioData;

 public:
  explicit WavHandler(std::string wavPath) {
    this->setWavInfo(wavPath);
  }
  void setWavInfo(std::string wavPath) {
    this->wavInfo = SndfileHandle(wavPath);

    if (this->wavInfo.error())
      throw std::runtime_error("LibSndFile error: " + std::string(this->wavInfo.strError()));

    this->extractAudioData();
  }

  size_t getSampleRate() { return this->wavInfo.samplerate(); }

  std::vector<double> getAudioData() { return std::move(this->audioData); }

 private:
  void extractAudioData() {
    sf_count_t readFrames;
    auto audioDataSize = this->wavInfo.frames() * this->wavInfo.channels();

    this->audioData = std::vector<double>(audioDataSize);
    readFrames = this->wavInfo.read(this->audioData.data(), audioDataSize);

    if (readFrames != audioDataSize)
      throw std::runtime_error("LibSndFile error: Couldn't read all the frames on wav file.");

    if (this->wavInfo.channels() > 1)
      this->convertToMono();

    if (this->audioData.empty())
      throw std::runtime_error("LibSndFile error: Failed to read audio file.");
  }

  void convertToMono() {
    auto frames = this->wavInfo.frames();
    auto channelsCount = this->wavInfo.channels();

    std::vector<double> monoAudio(frames);
    for (size_t frame = 0; frame != frames; ++frame) {
      for (size_t currentChannel = 0; currentChannel != channelsCount; ++currentChannel) {
        monoAudio[frame] += this->audioData[frame * channelsCount + currentChannel];
      }
      monoAudio[frame] /= channelsCount;
    }

    this->audioData = std::move(monoAudio);
  }
};

}

#endif //DICTAWAV_WAVHANDLER_H
