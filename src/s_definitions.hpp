#pragma once

#include "global.hpp"


// Definitions file. Contains the ID's of all instructions, and their blockSize.

namespace nthp {

    namespace script {

        struct Node_FileStructure_t {
            uint8_t ID;
            uint16_t blockSize;
        };
        struct Node_BinaryStructure_t {
            uint8_t ID;
            uint16_t blockSize;
            char* data;
        };

        union Node {
            Node_FileStructure_t file;
            Node_BinaryStructure_t bin;
        }



    }
}


