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
                constexpr uint16_t PaletteFileSize = (0b0000111111111111);
                constexpr size_t PaletteFileByteSize = PaletteFileSize * sizeof(nthp::texture::STPixelBinary);


                // A palette object used to generate softwareTextures. An array of 253 32-bit colors, with shifting operators.
                class Palette {
                public:
                        Palette();
                        Palette(const char* filename);


                        void importPaletteFromFile(const char* filename);
                        void clean(const nthp::texture::STPixelBinary p);
                        void exportPaletteToFile(const char* outputFile);

                        nthp::texture::Pixel pullColorSetWithAlpha(size_t colorIndex, uint8_t alpha) {
                                nthp::texture::Pixel pixel({colorSet[colorIndex].R, colorSet[colorIndex].G, colorSet[colorIndex].B, alpha});
                                return pixel;
                        }
                        
                        // Increases/Decreases every palette color's entire pixel RGB value by 'shiftLevel'
                        void whiteShift(int shiftLevel);

                        // Increases/Decreases every palette color's red pixel value by 'shiftLevel'
                        void redShift(int shiftLevel);

                        // Increases/Decreases every palette color's green pixel value by 'shiftLevel'
                        void greenShift(int shiftLevel);

                        // Increases/Decreases every palette color's blue pixel value by 'shiftLevel'
                        void blueShift(int shiftLevel);

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
