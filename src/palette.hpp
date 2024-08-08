#pragma once
#include "global.hpp"



namespace nthp {
        namespace texture {

                typedef uint32_t pixel_t;
                typedef uint8_t pixel_w;
                #define NTHPST_COLOR_WIDTH      uint16_t

                struct Pixel {
                        nthp::texture::pixel_t R : 8;
                        nthp::texture::pixel_t G : 8;
                        nthp::texture::pixel_t B : 8;
                        nthp::texture::pixel_t A : 8;
                };

                // Structure for softwareTexture Palette.
                struct STPixelBinary {
                        nthp::texture::pixel_w R;
                        nthp::texture::pixel_w G;
                        nthp::texture::pixel_w B;
                };
                constexpr auto PaletteFileSize = (0b0000111111111111);
                constexpr auto PaletteFileByteSize = PaletteFileSize * sizeof(nthp::texture::STPixelBinary);


                // A palette object used to generate softwareTextures. An array of 253 32-bit colors, with shifting operators.
                class Palette {
                public:
                        Palette();
                        Palette(const char* filename);


                        void importPaletteFromFile(const char* filename);
                        void clean(nthp::texture::Pixel s);
                        void exportPaletteToFile(const char* outputFile);

                        nthp::texture::Pixel pullColorSetWithAlpha(size_t colorIndex, uint8_t alpha) {
                                nthp::texture::Pixel pixel({colorSet[colorIndex].R, colorSet[colorIndex].G, colorSet[colorIndex].B, alpha});
                                return pixel;
                        }
                        

                        ~Palette();
                        STPixelBinary colorSet[nthp::texture::PaletteFileSize];
  

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