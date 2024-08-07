#include "rawsurface.hpp"

nthp::texture::rawSurface::rawSurface(uint32_t x, uint32_t y) {
        PRINT_DEBUG("Creating rawSurface...\n");

        surface = SDL_CreateRGBSurfaceWithFormat(0, x, y, 32, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32);
        if(surface == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
        }

        PRINT_DEBUG("Successfully created rawSurface at [%p]\n", surface);
}

// Sets a pixel on the surface to a color. Uses pixel binary position.
void nthp::texture::rawSurface::setPixel(size_t position, nthp::texture::Pixel pixel) {
        SDL_LockSurface(surface);

        uint8_t* pixels = (uint8_t*)surface->pixels;

        pixels[position * surface->format->BytesPerPixel] = pixel.R;
	pixels[position * surface->format->BytesPerPixel + 1] = pixel.G;
	pixels[position * surface->format->BytesPerPixel + 2] = pixel.B;
	pixels[position * surface->format->BytesPerPixel + 3] = pixel.A;

        SDL_UnlockSurface(surface);
}

// Sets a pixel on the surface to a color. Uses cartesien coordinates.
void nthp::texture::rawSurface::setPixel(uint32_t x, uint32_t y, nthp::texture::Pixel pixel) {
        SDL_LockSurface(surface);

        uint8_t* pixels = (uint8_t*)surface->pixels;

        pixels[y * surface->pitch + x * surface->format->BytesPerPixel] = pixel.R;
	pixels[y * surface->pitch + x * surface->format->BytesPerPixel + 1] = pixel.G;
	pixels[y * surface->pitch + x * surface->format->BytesPerPixel + 2] = pixel.B;
	pixels[y * surface->pitch + x * surface->format->BytesPerPixel + 3] = pixel.A;

        SDL_UnlockSurface(surface);
}


nthp::texture::Pixel nthp::texture::rawSurface::getPixel(size_t position) {
        nthp::texture::Pixel pixel;
        SDL_LockSurface(surface);

        uint8_t* pixels = (uint8_t*)surface->pixels;

        pixel.R = pixels[position * surface->format->BytesPerPixel];
	pixel.G = pixels[position * surface->format->BytesPerPixel + 1];
	pixel.B = pixels[position * surface->format->BytesPerPixel + 2];
	pixel.A = pixels[position * surface->format->BytesPerPixel + 3];

        SDL_UnlockSurface(surface);
        
        return pixel;
}

nthp::texture::rawSurface::~rawSurface() {
        SDL_FreeSurface(surface);
}