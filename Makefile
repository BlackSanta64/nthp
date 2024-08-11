TARGET = run
CC = g++
SRCDIR = src/


# The linux buildsystem relies on a standard installation of SDL2 and SDL2_image
# (should it be used), with whatever default directories come with, as decided
# by the package manager. This buildsystem has no flexibility in regards to library paths.
# To use a custom installation/build of SDL2, the whole buildsystem has to be update.


USE_SDLIMAGE = 0


ifeq ($(USE_SDLIMAGE), 1)

SDL_imageLibInclude = -lSDL2_image

else

SDL_imageLibInclude = 

endif


CFLAGS = -lSDL2 $(SDL_imageLibInclude) -D LINUX -D USE_SDLIMG=$(USE_SDLIMAGE)
DEBUG_CFLAGS = -g -lSDL2 $(SDL_imageLibInclude) -D LINUX -D DEBUG -DUSE_SDLIMG=$(USE_SDLIMAGE)


all: release debug


release: main.o global_defs.o core.o position.o palette.o rawsurface.o softwaretexture.o e_entity.o
	$(CC) main.o global_defs.o core.o position.o palette.o rawsurface.o softwaretexture.o e_entity.o $(CFLAGS) -o $(TARGET)


debug: main_d.o global_defs_d.o core_d.o position_d.o palette_d.o rawsurface_d.o softwaretexture_d.o e_entity_d.o
	$(CC) main_d.o global_defs_d.o core_d.o position_d.o palette_d.o rawsurface_d.o softwaretexture_d.o e_entity_d.o $(DEBUG_CFLAGS) -o $(TARGET)_debug


# RELEASE DEPENDENCIES GO HERE VVV

main.o: $(SRCDIR)main.cpp
	$(CC) $(CFLAGS) -c $(SRCDIR)main.cpp -o main.o

global_defs.o: $(SRCDIR)global_defs.cpp $(SRCDIR)global.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)global_defs.cpp -o global_defs.o

core.o: $(SRCDIR)core.cpp $(SRCDIR)core.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)core.cpp -o core.o

position.o: $(SRCDIR)position.cpp $(SRCDIR)position.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)position.cpp -o position.o


palette.o: $(SRCDIR)palette.cpp $(SRCDIR)palette.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)palette.cpp -o palette.o


rawsurface.o: $(SRCDIR)rawsurface.cpp $(SRCDIR)rawsurface.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)rawsurface.cpp -o rawsurface.o

softwaretexture.o: $(SRCDIR)softwaretexture.cpp $(SRCDIR)softwaretexture.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)softwaretexture.cpp -o softwaretexture.o

e_entity.o: $(SRCDIR)e_entity.cpp $(SRCDIR)e_entity.hpp
	$(CC) $(CFLAGS) -c $(SRCDIR)e_entity.cpp -o e_entity.o

# DEBUG DEPENDENCIES GO HERE VVV




main_d.o: $(SRCDIR)main.cpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)main.cpp -o main_d.o

global_defs_d.o: $(SRCDIR)global_defs.cpp $(SRCDIR)global.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)global_defs.cpp -o global_defs_d.o

core_d.o: $(SRCDIR)core.cpp $(SRCDIR)core.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)core.cpp -o core_d.o

position_d.o: $(SRCDIR)position.cpp $(SRCDIR)position.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)position.cpp -o position_d.o


palette_d.o: $(SRCDIR)palette.cpp $(SRCDIR)palette.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)palette.cpp -o palette_d.o


rawsurface_d.o: $(SRCDIR)rawsurface.cpp $(SRCDIR)rawsurface.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)rawsurface.cpp -o rawsurface_d.o

softwaretexture_d.o: $(SRCDIR)softwaretexture.cpp $(SRCDIR)softwaretexture.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)softwaretexture.cpp -o softwaretexture_d.o

e_entity_d.o: $(SRCDIR)e_entity.cpp $(SRCDIR)e_entity.hpp
	$(CC) $(DEBUG_CFLAGS) -c $(SRCDIR)e_entity.cpp -o e_entity_d.o







clean:
	@rm -f *.o *~ new_core.*

