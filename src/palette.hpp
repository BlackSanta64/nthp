#pragma once
#include "global.hpp"



namespace nthp {
        namespace texture {

                typedef uint32_t pixel_t;

                struct Pixel {
                        nthp::texture::pixel_t R : 8;
                        nthp::texture::pixel_t G : 8;
                        nthp::texture::pixel_t B : 8;
                        nthp::texture::pixel_t A : 8;
                };
                constexpr auto PaletteFileSize = 255;
                constexpr auto PaletteFileByteSize = 255 * sizeof(nthp::texture::Pixel);


                // A palette object used to generate softwareTextures. An array of 253 32-bit colors, with shifting operators.
                class Palette {
                public:
                        Palette();
                        Palette(const char* filename);


                        void importPaletteFromFile(const char* filename);
                        void clean();
                        void exportPaletteToFile(const char* outputFile);
                        void createEmptyPalette();
                        

                        ~Palette();
                        Pixel colorSet[nthp::texture::PaletteFileSize];
  

                };



        }
}