# DictaWav

DictaWav (utterances in latim) is a open source project of vocal utterances recognition. Based on Weightless Neural Network Model WiSARD (Wilkes, Stonham and Aleksander Recognition Device), it aims to be a general purpose library to the recognition of any set of vocal utterances from a single user.

### Requirements

#### CMake >= v3.5
DictaWav uses [CMake](https://cmake.org) as the project's build system

#### FFTW3
DictaWav uses [FFTW3](http://fftw.org/) to perform discrete Fourier transform on audio data, for install instructions, look at their page.

#### libsndfile
DictaWav uses [libsndfile](http://www.mega-nerd.com/libsndfile/) to manipulate wav files.

### Instructions


```
git clone https://github.com/fogodev/dictawav.git
cd dictawav
cmake .
make
./DictaWav
```
