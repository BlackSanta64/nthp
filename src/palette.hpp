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
                        void clean(nthp::texture::Pixel s);
                        void exportPaletteToFile(const char* outputFile);
                        

                        ~Palette();
                        Pixel colorSet[nthp::texture::PaletteFileSize];
  

                };

                namespace tools {


                #if USE_SDLIMG == 1
                        extern int generatePaletteFromImage(const char* inputImageFile, const char* outputFile);

                        // Defined in softwaretexture.cpp
                        extern int generateSoftwareTextureFromImage(const char* inputImageFile, const nthp::texture::Palette& palette, const char* outputFile);
                #endif



                }
        }
}