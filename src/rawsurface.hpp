#pragma once
#include "palette.hpp"


namespace nthp {


        namespace texture {
        // Just a wrapper of an SDL_Surface object with some convient features.
                class rawSurface {
                public:
                        rawSurface(uint32_t x, uint32_t y);
                        
                        void setPixel(size_t position, nthp::texture::Pixel pixel);
                        void setPixel(uint32_t x, uint32_t y, nthp::texture::Pixel pixel);

                        inline SDL_Surface* getSurface() { return surface; }


                        ~rawSurface();
                private:
                        SDL_Surface* surface;

                };
        }


}