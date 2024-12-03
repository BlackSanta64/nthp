#pragma once

#include "global.hpp"



namespace nthp {
        namespace audio {

                extern std::vector<std::string> audioDeviceNames;


                class MusicChannel {
                public:
                        MusicChannel() { musicData = NULL; }
                        MusicChannel(const char* file) {
                                load(file);
                        }

                        int load(const char* file) {
                                musicData = Mix_LoadMUS(file);
                                if(file == NULL) {
                                        PRINT_DEBUG_ERROR("Failed to load music data [%s]. %s\n", file, SDL_GetError());

                                        return -1;
                                }

                                return 0;
                        }

                        int start() {
                                if(Mix_PlayMusic(musicData, -1) == -1) {
                                        PRINT_DEBUG_ERROR("Failed to start music track at [%p]. %s\n", this, SDL_GetError());

                                        return -1;
                                }

                                return 0;
                        }

                        int stop() {
                                Mix_HaltMusic();

                                return 0;
                        }

                        int pauseMusic() {
                                if(Mix_PlayingMusic()) {
                                        Mix_PauseMusic();
                                }

                                return 0;
                        }

                        int resumeMusic() {
                                if(!Mix_PlayingMusic()) {
                                        Mix_ResumeMusic();
                                }

                                return 0;
                        }


                        ~MusicChannel() {
                                if(Mix_PlayingMusic()) {
                                        Mix_HaltMusic();
                                }

                                Mix_FreeMusic(musicData);
                        }

                        Mix_Music* musicData;

                };

                class SoundChannel {
                public:
                        SoundChannel() { soundData = NULL; }
                        SoundChannel(const char* file) {
                                load(file);
                        }
                        
                        int load(const char* file) {
                                soundData = Mix_LoadWAV(file);
                                if(soundData == NULL) {
                                        PRINT_DEBUG_ERROR("Unable to load sound [%s].\n", file);
                                }
                        }

                        int playSound() {
                                Mix_PlayChannel(-1, soundData, 0);
                        }


                        ~SoundChannel() {
                                Mix_FreeChunk(soundData);
                        }
                        Mix_Chunk* soundData;
                };




        }

}