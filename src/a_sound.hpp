#pragma once
#include "global.hpp"


// NTHP base sound system. The mainline audio driver and everything built on top uses
// the base 'Sound' object defined here.

namespace nthp {

        namespace audio {
                
                class Sound {
                public:
			Sound() { audioBuffer = nullptr; waveLength = 0; }
                        Sound(const char* wavFile);
						

                        ~Sound();


                        SDL_AudioSpec audioSpec;
                        uint8_t* audioBuffer;
                        uint32_t waveLength;
                };
        }
}
