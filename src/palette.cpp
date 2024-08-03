#include "palette.hpp"



nthp::texture::Palette::Palette() {
        colorSet = nullptr;
        colorSetSize = 0;
}


nthp::texture::Palette::Palette(const char* filename) {
        PRINT_DEBUG("Creating new Palette at [%p]...\n", this);
        colorSet = nullptr;
        colorSetSize = 0;

        this->importPaletteFromFile(filename);

        PRINT_DEBUG("Successfully created valid palette object at [%p]\n", this);
}


void nthp::texture::Palette::importPaletteFromFile(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG("Failed to create palette; file [%s] not found.\n", filename);
                colorSet = nullptr;
                colorSetSize = 0;

                return;
        }

        if(colorSetSize == 0) {
                // We use malloc here because we're cool as fuck.
                colorSet = (nthp::texture::Pixel*)malloc(nthp::texture::PaletteFileSize);
                colorSetSize = 255;
                memset(colorSet, 0, colorSetSize);
        
        }

        if(colorSet == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Unable to allocate memory for palette object.\n");
        }

        file.read((char*)colorSet, nthp::texture::PaletteFileSize);
        

        file.close();
        PRINT_DEBUG("Successfully imported color data into palette [%p]\n", this);
}



void nthp::texture::Palette::exportPaletteToFile(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::out | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG("Failed to export palette data to file; Unable to open target file.\n");
                return;
        }

        if(colorSetSize > 0)
                file.write((char*)colorSet, colorSetSize);

        file.close();

        PRINT_DEBUG("Successfully exported palette binary to file [%s].\n", filename);

}

void nthp::texture::Palette::createEmptyPalette() {
        if(colorSetSize > 0)
                memset(colorSet, '\000', colorSetSize);
        else {
                colorSetSize = 255;
                colorSet = (nthp::texture::Pixel*)malloc(colorSetSize);
                memset(colorSet, '\000', colorSetSize);
        }
}



nthp::texture::Palette::~Palette() {
        PRINT_DEBUG("Destroying Palette [%p]...\t", this); 
 
        if(colorSetSize > 0)
                free(colorSet);

        PRINT_DEBUG("done.\n");
}