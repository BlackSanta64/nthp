#pragma once

#include "s_node.hpp"
#include "s_instructions.hpp"
#include "e_entity.hpp"
#include "e_collision.hpp"
#include "gtexture.hpp"

namespace nthp { 
namespace script {

        extern uint8_t stageMemory[255];
        extern nthp::texture::Palette activePalette;

        class Script {
        public:

                struct Action {
                        nthp::script::instructions::indRef varIndex;
                        int boundKey = SDLK_UNKNOWN;
                };

        struct ScriptDataSet {
                        uint32_t localMemBudget;
                        uint32_t globalMemBudget;

                        uint32_t* currentLabelBlock;
                        uint32_t currentLabelBlockSize;

                        // Compiler-Evaluated.
                        //////////////////////////
                        nthp::script::Node* nodeSet;
                        size_t currentNode;


                        nthp::script::stdVarWidth* globalVarSet;
                        size_t varSetSize;
                        /////////////////////////

                        nthp::script::stdVarWidth** currentLocalMemory; // Changes when different scripts are executed.


                        nthp::texture::gTexture* textureBlock;
                        size_t textureBlockSize;

                        nthp::entity::gEntity* entityBlock;
                        size_t entityBlockSize;

                        nthp::texture::Frame* frameBlock;
                        size_t frameBlockSize;

                        nthp::script::scriptTriggerComplex currentTriggerConfig;

                        Action* actionList;
                        size_t  actionListSize;


                        bool isSuspended;

                };





                Script();
                Script(const char* filename, ScriptDataSet* dataSet);

                int import(const char* filename, ScriptDataSet* dataSet);

                int execute();
                int execute(uint32_t entryPoint);

                // Safe getter for any THPScript Var. To accurately pull VAR names, a debug compiler object must be used to query relative var indecies.
                inline nthp::script::stdVarWidth getVar(size_t index) { if(index < localVarSetSize) return localVarSet[index]; else return 0; }
                

                
                ScriptDataSet* getScriptData() { return data; }

                ~Script();
        private:
                mutable ScriptDataSet* data;
                
                nthp::script::stdVarWidth* localVarSet;
                size_t localVarSetSize;

                uint32_t* localLabelBlock;
                uint32_t localLabelBlockSize;

                size_t localCurrentNode;
                std::vector<nthp::script::Node> nodeSet;

                bool inStageContext;
        };


}

}