#include "pm.hpp"



// Linked with pm_core.cpp. Only provides externs and symbols to core_pm.
// nothing to see here.




void PM_PRINT_ERROR(const char* format, ...) {
	va_list ap;
	
	va_start(ap, format);

	printf(" ERROR: ", SDL_GetTicks());	
	printf(format, ap);


	va_end(ap);
}


