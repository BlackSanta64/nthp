#define LINUX
#define DEBUG
#include "global.hpp"

volatile uint32_t nthp::deltaTime;
volatile nthp::vect32 nthp::mousePosition;

FILE* NTHP_debug_output = stdout;

void nthp::THROW_FATAL(char errorcode, const char* fatal_message) {
        printf("[%u] FATAL: %s\n", SDL_GetTicks(), fatal_message);

        throw FATAL_ERROR(errorcode);
}


#ifdef DEBUG
void PRINT_DEBUG(const char* format, ...) {

	va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}



#endif
