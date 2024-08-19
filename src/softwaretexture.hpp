#pragma once
#include "rawsurface.hpp"

namespace nthp {
        namespace texture {
                extern const uint8_t STheaderSignature;
                class SoftwareTexture {
                public:
                        SoftwareTexture();
                        SoftwareTexture(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);
                        SoftwareTexture(SDL_Surface* surface, nthp::texture::Palette* palette, SDL_Renderer* renderer);

                        int generateTexture(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);
                        inline SDL_Texture* getTexture() { return texture; }

                        void regenerateTexture(nthp::texture::Palette* palette, SDL_Renderer* renderer);
                        
                        
                        struct software_texture_header {
                                uint8_t signature;
                                uint32_t x;
                                uint32_t y;
                        };

                        #define NTHPST_ALPHAMASK        0b0000000000001111
                        #define NTHPST_COLORMASK        0b1111111111110000
                        
                        static constexpr NTHPST_COLOR_WIDTH alphaLevelSize = UINT8_MAX / NTHPST_ALPHAMASK; 



                        ~SoftwareTexture();
                private:


                        SDL_Texture* texture;


                       
                        

                        constexpr NTHPST_COLOR_WIDTH getPixelColor(NTHPST_COLOR_WIDTH pixel)        { return ((pixel & NTHPST_COLORMASK) >> 4); }
                        constexpr NTHPST_COLOR_WIDTH getPixelAlphaLevel(NTHPST_COLOR_WIDTH pixel)        { return (pixel & NTHPST_ALPHAMASK); }
                        constexpr NTHPST_COLOR_WIDTH getTrueAlpha(NTHPST_COLOR_WIDTH alphaLevel)    { return (alphaLevel * alphaLevelSize); }

                        NTHPST_COLOR_WIDTH* pixelData;
                        size_t dataSize;
                        software_texture_header metadata;
                };
                namespace tools {
                

                #if USE_SDLIMG == 1
                        extern int generatePaletteFromImage(const char* inputImageFile, const char* outputFile);
                        extern int generateSoftwareTextureFromImage(const char* inputImageFile, nthp::texture::Palette* palette, const char* outputFile);

                #else
                        #pragma message("WARNING: nthp::texture::tools utilities ignored; Dependency 'SDL_image' excluded.")
                        #define generateSoftwareTextureFromImage(...)
                        #define generatePaletteFromImage(...)
                #endif
                }

                struct Frame {
                        SDL_Texture*    texture;
                        SDL_Rect        src;
                };


                
        }
}
