#include "softwaretexture.hpp"

const uint8_t nthp::texture::STheaderSignature = 0b11011001;

nthp::texture::SoftwareTexture::SoftwareTexture() {
        pixelData = nullptr;
        texture = nullptr;
        dataSize = 0;
}




nthp::texture::SoftwareTexture::SoftwareTexture(const char* filename, const nthp::texture::Palette& palette, SDL_Renderer* coreRenderer) {
        pixelData = nullptr;
        texture = nullptr;
        dataSize = 0;
        
        if(this->generateTexture(filename, palette, coreRenderer)) {

        }
}








// Generates a texture from a valid ST file with the specified palette. Returns a 1 on failure. Safe to run
// when a valid texture is already generated.
int nthp::texture::SoftwareTexture::generateTexture(const char* filename, const nthp::texture::Palette& palette, SDL_Renderer* coreRenderer) {
        if(dataSize != 0) { SDL_DestroyTexture(texture); free(pixelData); dataSize = 0; }
        PRINT_DEBUG("Creating SoftwareTexture...\n");
        
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG("Unable to create SoftwareTexture; file not found.\n");
                pixelData = nullptr;
                dataSize = 0;

                return 1;
        }


        file.read((char*)&metadata, sizeof(metadata));

        if(metadata.signature != STheaderSignature) {
                PRINT_DEBUG("Unable to create SoftwareTexture; Invalid file format.\n");
                pixelData = nullptr;
                dataSize = 0;

                return 1;
        }

        dataSize = metadata.x * metadata.y;
        pixelData = (decltype(pixelData))malloc(dataSize);

        if(pixelData == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Unable to allocate pixelData buffer.");
        }

        file.read((char*)pixelData, dataSize);
        nthp::texture::rawSurface stSurface(metadata.x, metadata.y);


        // Loops through and generates the texture using the given palette.
        for(size_t i = 0; i < dataSize; ++i)
                stSurface.setPixel(i, palette.colorSet[pixelData[i]]);

        texture = SDL_CreateTextureFromSurface(coreRenderer, stSurface.getSurface());

        return 0;
}


// Uses saved pixeldata to redraw a texture with a different palette.
void nthp::texture::SoftwareTexture::regenerateTexture(const nthp::texture::Palette& palette, SDL_Renderer* renderer) {
        if(dataSize != 0) 
                SDL_DestroyTexture(texture);
        else {
                PRINT_DEBUG("Unable to regenerate software texture [%p]; Texture not loaded.\n", this);
        }

        nthp::texture::rawSurface stSurface(metadata.x, metadata.y);

        for(size_t i = 0; i < dataSize; ++i)
                stSurface.setPixel(i, palette.colorSet[pixelData[i]]);

        texture = SDL_CreateTextureFromSurface(renderer, stSurface.getSurface());

        PRINT_DEBUG("Regenerated texture [%p] with palette [%p].\n", this, palette);
}



nthp::texture::SoftwareTexture::~SoftwareTexture() {
        PRINT_DEBUG("Destroying SoftwareTexture [%p]...\t", this);

        SDL_DestroyTexture(texture);
        
        if(dataSize > 0)
                free(pixelData);

        PRINT_DEBUG("done.\n");
}