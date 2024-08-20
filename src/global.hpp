#pragma once

#ifdef LINUX
	#include <SDL2/SDL.h>
#endif

#ifdef WINDOWS
	#include <SDL.h>
#endif

#if USE_SDLIMG == 1
#ifdef WINDOWS
    	#include <SDL_image.h>
#endif
#ifdef LINUX
	#include <SDL2/SDL_image.h>
#endif

#endif



#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <cstdarg>
#include <ctime>
#include <vector>
#include <fstream>
#include <stdio.h>
#include "fixed.hpp"



#ifndef SILENCE_DEBUG

// Use DEBUG_PRINT as a regular printf wrapper. It gets substituted out
// if unless DEBUG is defined 
        #ifdef DEBUG

	        extern FILE* NTHP_debug_output;
	        extern void PRINT_DEBUG                 (const char* format, ...);
                extern void PRINT_DEBUG_ERROR           (const char* format, ...);

        
                #define NOVERB_PRINT_DEBUG(...)         fprintf(NTHP_debug_output, __VA_ARGS__)
                #define GENERIC_PRINT(...)              printf(__VA_ARGS__)

                #else
        // All arguments are substituted out when DEBUG is not defined.
        // The debug build config defines DEBUG in all translation units, but all
        // debug functions are valid in the release build, so debugging individual source
        // files is possible if DEBUG is defined before any includes.

                #define PRINT_DEBUG(...)
                #define NOVERB_PRINT_DEBUG(...)
                #define GENERIC_PRINT(...)
                #define PRINT_DEBUG_ERROR(...)

        #endif



#else

        #define PRINT_DEBUG(...)
        #define NOVERB_PRINT_DEBUG(...)
        #define GENERIC_PRINT(...)
        #define PRINT_DEBUG_ERROR(...)

#endif

        // Must be executed first to ensure a valid file descriptor.
        // Specifies a target for the debug output system. Default is 'stdout'
        extern int NTHP_GEN_DEBUG_INIT(FILE* fdescriptor);

        // Can be executed whenever; Ties up loose ends in the debugging system.
        extern void NTHP_GEN_DEBUG_CLOSE(void);

namespace nthp {
        typedef enum {
                SDL_Failure,
                Memory_Fault,
                CriticalFileFailure
        } FATAL_ERROR;

        extern void THROW_FATAL(char errorcode, const char* fatal_message);

        
        // Outputs a message and crashes the program. Use nthp::FATAL_ERROR values for
        // errorcode corresponding to the fatal fault.
        #define FATAL_PRINT(errorcode, message) nthp::THROW_FATAL(errorcode, message)

    
        template<class T>
        class vector {
        public:
                vector() { x = 0; y = 0; };
                vector(T x, T y) { this->x = x; this->y = y; };
                
                vector<T> operator+(vector<T> a) { return vector<T>(x + a.x, y + a.y); }
                vector<T> operator-(vector<T> a) { return vector<T>(x - a.x, y - a.y); }
                void operator+=(vector<T> a) { x += a.x; y += a.y; }
                void operator-=(vector<T> a) { x -= a.x; y -= a.y; }

                T x;
                T y;
        };

        typedef vector<int32_t> vect32;
        typedef vector<int64_t> vect64;

        typedef vector<float> vectf32;
        typedef vector<double> vectf64;

        typedef vector<fixed_t> vectFixed;

        // The number of milliseconds the previous frame took to complete (logic and rendering)
        extern fixed_t deltaTime;

        // The maximum time a frame can take in milliseconds. Set to -1 to disable.
        extern fixed_t frameDelay;

        // The mouse's current WorldPosition (not pixel position, be careful) relative to the active window
        extern vectFixed mousePosition;

        // Sets the max framerate in FPS to nthp::frameDelay
        inline void setMaxFPS(const FIXED_TYPE fps) { frameDelay = nthp::f_fixedQuotient(nthp::intToFixed(1000), nthp::intToFixed(fps)); }


        template<class T>
        class sArray {
        public:
		sArray() { array = nullptr; size = 0; }
                sArray(size_t size) {
                        array = new T[size];
                        this->size = size;
                }

                T& operator[](size_t index) {
                        if(index > size) PRINT_DEBUG_ERROR("Segmentation Fault in sArray.");
                        return array[index];
                }
                inline const T* getData() { return array; }
                inline size_t getSize() { return size; }

		void alloc(size_t t_size) { array = new T[t_size]; this->size = t_size; }
		void dealloc() { delete[] array; size = 0; }

                inline size_t getBinarySize() { return (size * sizeof(T)); }

                ~sArray() { if(size > 0) delete[] array; }
        private:
                T* array;
                size_t size;
        };



}


// Global header file; definitions shared across all source files.
// Try to only add what's nessesary.

