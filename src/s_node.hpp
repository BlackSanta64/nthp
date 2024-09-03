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

        template<class T>
        struct P_Reference {
                T value;
                uint8_t metadata = 0; // See below

                // Using some shift/mask/cast shit instead of bit-field for P_Reference
                // metadata. Bit-fields do not mix with files.

        };

        typedef enum __P_REF_FLAGS_BITS {
                IS_REFERENCE,
                IS_GLOBAL
        } flagBits;


        #define PR_METADATA_SET(varRef, flag)           ((varRef.metadata) |= ( 1 <<( (int) flag )))
        #define PR_METADATA_CLEAR(varRef, flag)         ((varRef.metadata) &= ~( 1 <<( (int) flag )))
        #define PR_METADATA_GET(varRef, flag)           ((ref.metadata >> ((int) flag )) & 1)

}
}