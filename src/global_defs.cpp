#ifndef DEBUG
        #define DEBUG
#endif

#include "global.hpp"

volatile uint32_t nthp::deltaTime;
volatile nthp::vect32 nthp::mousePosition;

FILE* NTHP_debug_output = stdout;

void nthp::THROW_FATAL(char errorcode, const char* fatal_message) {
        printf("[%u] FATAL: %s\n", SDL_GetTicks(), fatal_message);

        throw FATAL_ERROR(errorcode);
}


void PRINT_DEBUG(const char* format, ...) {

	va_list ap;
	
	va_start(ap, format);

	fprintf(NTHP_debug_output, "[%u] DEBUG: ", SDL_GetTicks());	
	vfprintf(NTHP_debug_output, format, ap);


	va_end(ap);
}


int NTHP_GEN_DEBUG_INIT(FILE* fdescriptor) {
        if(fdescriptor == NULL) {
                NTHP_debug_output = stdout;
                return -1;
        }

        NTHP_debug_output = fdescriptor;


        time_t ti = time(NULL);

        // I know how it looks, but 'localtime' is statically allocated,
        // so this is not a memory leak.
        struct tm *info = localtime(&ti);


        fprintf(NTHP_debug_output, "\tNTHP Debug Session Start : %s\n", asctime(info));
        return 0;
}

void NTHP_GEN_DEBUG_CLOSE(void) {
        time_t ti = time(NULL);

        // I know how it looks, but 'localtime' is statically allocated,
        // so this is not a memory leak.
        struct tm *info = localtime(&ti);
        fprintf(NTHP_debug_output, "\n\tNTHP Debug Session End : %s", asctime(info));


        if(NTHP_debug_output != stdout) {
                fclose(NTHP_debug_output);
        }
}

