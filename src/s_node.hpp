#pragma once
#include "global.hpp"

namespace nthp {
namespace script {

        // The base unit of script execution. A Node contains an 8bit ID, defining instruction type.

        union Node {
        public:
                struct n_file_t {
                        uint8_t ID;
                        uint8_t size;
                } 

                file_p;
                struct n_binary_t {
                        uint8_t ID;
                        uint8_t size;
                        char* data;
                }
                
                access;

                Node() { access = {0,0,NULL}; }
                Node(uint8_t _ID, uint8_t dataSize) { access.ID = _ID; access.size = dataSize; }

        };

        inline void cleanNodeSet(std::vector<Node>& set) {
                for(size_t i = 0; i < set.size(); ++i) {
                        free(set[i].access.data);
                }
                set.clear();
        }

        
                        // Use this to work with script triggers, but read and write from files with the
                        // 'trigger_w' 
        struct scriptTriggerComplex {
                uint16_t ID : 3;
                uint16_t GPR : 5;
                uint16_t MEM : 8;
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
                IS_GLOBAL,
                IS_VALID,
                IS_NEGATED,
                IS_PTR          // IS_PTR is checked FIRST!
        } flagBits;


        #define PR_METADATA_SET(varRef, flag)           ((varRef.metadata) |= ( 1 <<( (int) flag )))
        #define PR_METADATA_CLEAR(varRef, flag)         ((varRef.metadata) &= ~( 1 <<( (int) flag )))
        #define PR_METADATA_GET(varRef, flag)           ((varRef.metadata >> ((int) flag )) & 1)

}
}