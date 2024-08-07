#include "rawsurface.hpp"

namespace nthp {
        namespace texture {
                extern const uint8_t STheaderSignature;
                class SoftwareTexture {
                public:
                        SoftwareTexture();
                        SoftwareTexture(const char* filename, const nthp::texture::Palette& palette, SDL_Renderer* coreRenderer);
                        SoftwareTexture(SDL_Surface* surface, const nthp::texture::Palette& palette, SDL_Renderer* renderer);

                        int generateTexture(const char* filename, const nthp::texture::Palette& palette, SDL_Renderer* coreRenderer);
                        inline SDL_Texture* getTexture() { return texture; }

                        void regenerateTexture(const nthp::texture::Palette& palette, SDL_Renderer* renderer);
                        
                        
                        struct software_texture_header {
                                uint8_t signature;
                                uint32_t x;
                                uint32_t y;
                        };



                        ~SoftwareTexture();
                private:


                        SDL_Texture* texture;

                        uint8_t* pixelData;
                        size_t dataSize;
                        software_texture_header metadata;
                };
                namespace tools {
                #if USE_SDLIMG == 1
                        extern int generatePaletteFromImage(const char* inputImageFile, const char* outputFile);
                        extern int generateSoftwareTextureFromImage(const char* inputImageFile, const nthp::texture::Palette& palette, const char* outputFile);
                #endif
                }




                
        }
}