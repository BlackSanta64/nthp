#pragma once

#include "global.hpp"
#include "a_sound.hpp"


namespace nthp {
	namespace audio {


		// The Driver handles mixing and playing the audio queue,
		// and switching audio devices. Takes and plays 'nthp::audio::Sound'.
		// I hate this.

		// Using sArray because it cleans itself up when the program ends.

		extern nthp::sArray<std::string> AudioDeviceList;
		
		// Fetches the system's native audio device list and loads them into
		// nthp::audio::AudioDeviceList.
		extern int NTHP_AUDIO_INIT(void);

	
		class Driver {
		public:
			Driver();

			void setAudioDevice(const char* device);
			void setPlayback(int status);

			void playSound(size_t sIndex);


		private:
			SDL_AudioDeviceID selectedDevice;
			
			nthp::audio::Sound* soundBank;
			size_t soundBankSize;

		};

	}
}
