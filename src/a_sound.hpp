#pragma once
#include "global.hpp"


// NTHP base sound system. The mainline audio driver and everything built on top uses
// the base 'Sound' object defined here.

namespace nthp {

        namespace audio {
                
                class Sound {
                public:
                        Sound(const char* wavFile);

                        ~Sound();
                private:
                        SDL_AudioDeviceID audioDevice;

                        SDL_AudioSpec audioSpec;
                        uint8_t* audioStart;
                        uint32_t waveLength;
                };
        }
}