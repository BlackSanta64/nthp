#include "palette.hpp"



nthp::texture::Palette::Palette() {
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);
}


nthp::texture::Palette::Palette(const char* filename) {
        PRINT_DEBUG("Creating new Palette at [%p]...\n", this);
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);

        if(this->importPaletteFromFile(filename)) { };

        PRINT_DEBUG("Successfully created valid palette object at [%p]\n", this);
}


int nthp::texture::Palette::importPaletteFromFile(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Failed to create palette; file [%s] not found.\n", filename);

                return 1;
        }

        if(colorSet == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::Memory_Fault, "Unable to allocate memory for palette object.\n");
                return 1;
        }

        
        file.read((char*)colorSet, nthp::texture::PaletteFileByteSize);
        

        file.close();
        PRINT_DEBUG("Successfully imported color data into palette [%p]\n", this);
        return 0;
}



int nthp::texture::Palette::exportPaletteToFile(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::out | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Failed to export palette data to file; Unable to open target file.\n");
                return 1;
        }


        file.write((char*)colorSet, nthp::texture::PaletteFileByteSize);

        file.close();

        PRINT_DEBUG("Successfully exported palette binary to file [%s].\n", filename);
        return 0;
}

void nthp::texture::Palette::clean(const nthp::texture::STPixelBinary p) {
        memset(colorSet, 0, nthp::texture::PaletteFileByteSize);
}

void nthp::texture::Palette::whiteShift(int shiftLevel) {
        for(int i = 0; i < nthp::texture::PaletteFileSize; ++i) {
                colorSet[i].R += shiftLevel;
                colorSet[i].G += shiftLevel;
                colorSet[i].B += shiftLevel;
        }
}

void nthp::texture::Palette::redShift(int shiftLevel) {
        for(int i = 0; i < nthp::texture::PaletteFileSize; ++i) {
                colorSet[i].R += shiftLevel;
        }
}

void nthp::texture::Palette::greenShift(int shiftLevel) {
        for(int i = 0; i < nthp::texture::PaletteFileSize; ++i) {
                colorSet[i].G += shiftLevel;
        }
}

void nthp::texture::Palette::blueShift(int shiftLevel) {
        for(int i = 0; i < nthp::texture::PaletteFileSize; ++i) {
                colorSet[i].B += shiftLevel;
        }
}



nthp::texture::Palette::~Palette() {

}




