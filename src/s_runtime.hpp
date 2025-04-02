#pragma once

#include "s_script.hpp"
#include "s_linker.hpp"



namespace nthp {
        namespace script {

                class Runtime {
                public:
                        Runtime();
                        Runtime(const char* filename);

                        int importExecutable(const char* filename);
                        void handleEvents();

                        void clean();

                        int execTick();
                        int execInit();
                        int execExit();

                        nthp::script::Script::ScriptDataSet data;

                        ~Runtime();
                private:
                        std::vector<uint32_t> initList;
                        std::vector<uint32_t> tickList;
                        std::vector<uint32_t> exitList;

                        std::vector<uint32_t> allHeaderLocations;
                        std::vector<nthp::script::Script> scriptData;
                };

        }
}