#include "global.hpp"

volatile uint32_t nthp::deltaTime;
volatile nthp::vect32 nthp::mousePosition;



void nthp::THROW_FATAL(char errorcode, const char* fatal_message) {
        printf("[%u] FATAL: %s\n", SDL_GetTicks(), fatal_message);

        throw FATAL_ERROR(errorcode);
}