#pragma once
#define LINUX
#ifdef LINUX
	#include <SDL2/SDL.h>
#endif

#ifdef WINDOWS
	#include <SDL.h>
#endif

#if USE_SDLIMG == 1
    #include <SDL_image.h>
#endif



#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdint.h>
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
                Memory_Fault,
                sArray_Segfault
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

        extern volatile uint32_t deltaTime;
        extern volatile vect32 mousePosition; 


        template<class T>
        class sArray {
        public:
                sArray(size_t size) {
                        array = new T[size];
                        this->size = size;
                }

                T& operator[](size_t index) {
                        if(index > size) FATAL_PRINT(nthp::FATAL_ERROR::sArray_Segfault, "Segmentation Fault in sArray.");
                        return array[index];
                }
                inline const T* getData() { return array; }
                inline size_t getSize() { return size; }

                inline size_t getBinarySize() { return (size * sizeof(T)); }

                ~sArray() { if(size > 0) delete[] array; }
        private:
                T* array;
                size_t size;
        };



}


// Global header file; definitions shared across all source files.
// Try to only add what's nessesary.

