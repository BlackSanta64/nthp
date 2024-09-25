#include "st_compress.hpp"


namespace nthp {
        namespace texture {
                class gTexture {
                public:
                        gTexture();
                        gTexture(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);
                        

                        int autoLoadTextureFile(const char* filename, nthp::texture::Palette* palette, SDL_Renderer* coreRenderer);
                        const nthp::texture::SoftwareTexture& getTextureData() { return texture; }
                        
                private:
                        int decompressFile(const char* filename);
                        nthp::texture::SoftwareTexture texture;

                };
        }
}