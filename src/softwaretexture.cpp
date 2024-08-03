#include "softwaretexture.hpp"

const uint8_t headerSignature = 0b11011001;

nthp::texture::SoftwareTexture::SoftwareTexture() {
        pixelData = nullptr;
        texture = nullptr;
        dataSize = 0;
}

nthp::texture::SoftwareTexture::SoftwareTexture(const char* filename, const nthp::texture::Palette& palette, SDL_Renderer* coreRenderer) {
        if(this->generateTexture(filename, palette, coreRenderer)) {
                pixelData = nullptr;
                texture = nullptr;
                dataSize = 0;
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

        nthp::texture::SoftwareTexture::software_texture_header header;
        file.read((char*)&header, sizeof(header));

        if(header.signature != headerSignature) {
                PRINT_DEBUG("Unable to create SoftwareTexture; Invalid file format.\n");
                pixelData = nullptr;
                dataSize = 0;

                return 1;
        }

        size_t dataSetSize = header.x * header.y;
        pixelData = (decltype(pixelData))malloc(dataSetSize);

        if(pixelData == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Unable to allocate pixelData buffer.");
        }

        file.read((char*)pixelData, dataSetSize);
        nthp::texture::rawSurface stSurface(header.x, header.y);


        // Loops through and generates the texture using the given palette.
        for(size_t i = 0; i < dataSetSize; ++i)
                stSurface.setPixel(i, palette.colorSet[pixelData[i]]);

        texture = SDL_CreateTextureFromSurface(coreRenderer, stSurface.getSurface());

        return 0;
}


nthp::texture::SoftwareTexture::~SoftwareTexture() {
        PRINT_DEBUG("Destroying SoftwareTexture [%p]...\t", this);

        SDL_DestroyTexture(texture);
        
        if(dataSize > 0)
                free(pixelData);

        PRINT_DEBUG("done.\n");
}