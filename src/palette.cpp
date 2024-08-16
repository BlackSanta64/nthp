#include "palette.hpp"



nthp::texture::Palette::Palette() {
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);
}


nthp::texture::Palette::Palette(const char* filename) {
        PRINT_DEBUG("Creating new Palette at [%p]...\n", this);
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);

        this->importPaletteFromFile(filename);

        PRINT_DEBUG("Successfully created valid palette object at [%p]\n", this);
}


void nthp::texture::Palette::importPaletteFromFile(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG("Failed to create palette; file [%s] not found.\n", filename);

                return;
        }

        if(colorSet == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Unable to allocate memory for palette object.\n");
        }

        
        file.read((char*)colorSet, nthp::texture::PaletteFileByteSize);
        

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


        file.write((char*)colorSet, nthp::texture::PaletteFileByteSize);

        file.close();

        PRINT_DEBUG("Successfully exported palette binary to file [%s].\n", filename);

}

void nthp::texture::Palette::clean(const nthp::texture::Pixel p) {
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);
}



nthp::texture::Palette::~Palette() {
        PRINT_DEBUG("Destroying Palette [%p]...\t", this); 


        NOVERB_PRINT_DEBUG("done.\n");
}




