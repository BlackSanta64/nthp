#pragma once

#include "s_node.hpp"
#include "s_instructions.hpp"
#include "e_entity.hpp"
#include "e_collision.hpp"
#include "gtexture.hpp"

namespace nthp { 
namespace script {


        class Script {
        public:
                Script();
                Script(const char* filename);

                int import(const char* filename);

                int execute();
                int execute(uint32_t entryPoint);

                inline nthp::script::stdVarWidth getVar(size_t index) { if(index < data.varSetSize) return data.varSet[index]; else return 0; }
                

                struct ScriptDataSet {
                        uint32_t localMemBudget;
                        uint32_t globalMemBudget;

                        uint32_t* labelBlock;
                        uint32_t labelBlockSize;

                        // Compiler-Evaluated.
                        //////////////////////////
                        std::vector<nthp::script::Node> nodeSet;
                        size_t currentNode;


                        nthp::script::stdVarWidth* varSet;
                        size_t varSetSize;
                        /////////////////////////


                        nthp::texture::gTexture* textureBlock;
                        size_t textureBlockSize;

                        nthp::entity::gEntity* entityBlock;
                        size_t entityBlockSize;

                        nthp::texture::Frame* frameBlock;
                        size_t frameBlockSize;

                        nthp::texture::Palette activePalette;

                        bool isSuspended;

                };

                inline ScriptDataSet* getScriptData() { return &data; }

                ~Script();
        private:
                ScriptDataSet data;
        };


}

}