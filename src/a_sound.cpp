#include "a_sound.hpp"



nthp::audio::Sound::Sound(const char* waveFile) {
        
        if(SDL_LoadWAV(waveFile, &audioSpec, &audioStart, &waveLength) == NULL) {
                PRINT_DEBUG_ERROR("Failed to import WAV file.");
                return;
        }

}