#pragma once
#include "s_compiler.hpp"






namespace nthp { 
        namespace script {


                // The Linker object combines many compiled source files (.thpcs) from a build system
                // into a single executable script. Function calls and FUNCs can only work in a script that has
                // been linked to another with the required definitions.
                //
                // Execution order is saved to the headers of each script, as well as GOTO offsets from the compiler.
                // GOTO stores "offset location relative to header" instead of real positions. All linked script
                // header locations are stored in runtime.
                class LinkerInstance {
                public:
                        LinkerInstance() { scriptData = NULL; }
                        LinkerInstance(std::vector<std::string>& targets, const char* output);

                        int linkFiles(std::vector<std::string>& targets, const char* output);

                        static constexpr uint16_t ID = 0b1010110101110101;
                        struct link_header {
                                uint16_t HeaderID = ID;
                                uint16_t scriptCount;
                                uint32_t totalNodeCount;
                        };


                        ~LinkerInstance();
                private:
                        std::vector<nthp::script::Node>* scriptData;
                        std::vector<std::string> targetFiles;
                };





        }
}