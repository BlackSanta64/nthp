#include "t_font.hpp"



nthp::text::Font::Font() {
        memset(fontData, 0, nthp::text::fontByteLength);
}



int nthp::text::Font::importFont(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open font file [%s]; File inaccessible.\n", filename);
                return 1;
        }

        file.read((char*)fontData, nthp::text::fontByteLength);
        file.close();

        return 0;
}


int nthp::text::Font::exportFont(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::out | std::ios::binary);

        if(file.fail()) {
                PRINT_DEBUG_ERROR("Unable to open font file [%s]; Permission denied.\n", filename);
                return 1;
        }

        file.write((char*)&fontData, nthp::text::fontByteLength);
        file.close();
        
        return 0;
}