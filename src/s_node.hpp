#pragma once
#include "global.hpp"

namespace nthp {
namespace script {

        // The base unit of script execution. A Node contains an 8bit ID, defining instruction type.

        union Node {
        public:
                struct n_file_t {
                        uint8_t ID;
                        uint32_t size;
                } 

                file_p;
                struct n_binary_t {
                        uint8_t ID;
                        uint32_t size;
                        char* data;
                }
                
                access;

                Node() { access = {0,0,NULL}; }
                Node(uint8_t _ID, uint32_t dataSize) { access.ID = _ID; access.size = dataSize; }

        };

        constexpr size_t NodeSize = sizeof(Node);
        typedef nthp::fixed_t stdVarWidth;

}
}