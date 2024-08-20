#include "a_sound.hpp"



nthp::audio::Sound::Sound(const char* wavFile) {
	if(SDL_LoadWAV(wavFile, &audioSpec, &audioBuffer, &waveLength)) {
		PRINT_DEBUG_ERROR("Unable to import WAV file [%s].", wavFile);
		
		waveLength = 0;
		return;
	}
}






// The only cleanup is the wave data allocated by loadWAV.
// the rest is static.
nthp::audio::Sound::~Sound() {
	if(waveLength > 0)
		SDL_FreeWAV(audioBuffer);
}
