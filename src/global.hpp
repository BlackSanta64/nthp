#pragma once

#include <SDL.h>

#if USE_SDLIMG == 1
    #include <SDL_image.h>
#endif



#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdarg>
#include <vector>
#include <fstream>
#include "fixed.hpp"





// Use DEBUG_PRINT as a regular printf wrapper. It gets substituted out
// if unless DEBUG is defined 
#ifdef DEBUG
        #define PRINT_DEBUG(...)        printf(__VA_ARGS__)
#else
        #define DEBUG                   0
        #define PRINT_DEBUG(...) 

#endif

namespace nthp {
        typedef enum {
                SDL_Failure,
                Memory_Fault
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
                
                T x;
                T y;
        };

        typedef vector<int32_t> vect32;
        typedef vector<int64_t> vect64;

        typedef vector<float_t> vectf32;
        typedef vector<double_t> vectf64;

        typedef vector<fixed_t> vectFixed;

        extern volatile uint32_t deltaTime;
        extern volatile vect32 mousePosition; 



}


// Global header file; definitions shared across all source files.
// Try to only add what's nessesary.

