#pragma once
#include "rawsurface.hpp"

namespace nthp {
        namespace texture {
                


                class SoftwareTexture {
                public:
                        SoftwareTexture();
                        SoftwareTexture(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);

                        // Read ST data from a valid ST file. Does NOT generate a valid rendering texture as no palette is given.
                        // Must use 'regenerateTexture()' with a palette to generate.
                        SoftwareTexture(const char* filename);


                        int generateTexture(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);
                        inline SDL_Texture* getTexture() { return texture; }

                        void regenerateTexture(nthp::texture::Palette* palette, SDL_Renderer* renderer);
                        
                        static const uint8_t STheaderSignature = 0b11011001;
                        struct software_texture_header {
                                uint8_t signature;
                                uint32_t x;
                                uint32_t y;
                        };

                        #define NTHPST_ALPHAMASK        0b0000000000001111
                        #define NTHPST_COLORMASK        0b1111111111110000
                        
                        static constexpr NTHPST_COLOR_WIDTH alphaLevelSize = UINT8_MAX / NTHPST_ALPHAMASK; 

                        NTHPST_COLOR_WIDTH* getPixelData() { return pixelData; }
                        const size_t getPixelDataSize() { return dataSize; }

                        void createEmptyTexture(const size_t dataSize);
                        const software_texture_header getMetaData() { return metadata; }
                        inline void manual_metadata_override(const software_texture_header _ovr) { metadata = _ovr; dataSize = metadata.x * metadata.y; }

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
                        #warning("WARNING: nthp::texture::tools utilities ignored; Dependency 'SDL_image' excluded.")
                #endif
                }

                struct Frame {
                        SDL_Texture*    texture;
                        SDL_Rect        src;
                };


                
        }
}
