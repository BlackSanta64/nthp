#pragma once

#include "s_node.hpp"
#include "s_instructions.hpp"
#include "e_entity.hpp"
#include "e_collision.hpp"
#include "gtexture.hpp"

namespace nthp { 
namespace script {

                #define STAGEMEM_MAX 255

        extern char stageMemory[STAGEMEM_MAX];


        namespace debug {
        #ifdef DEBUG
                extern nthp::vectGeneric debugInstructionCall; // Interface with the project manager debug system. x = instruction y = data
                extern bool suspendExecution;

                typedef enum {
                        BREAK,
                        CONTINUE,
                        JUMP_TO,
                        STEP,
                        NEXT_PHASE
                } DEBUG_CALLS;


                typedef enum {
                        INIT,
                        TICK,
                        LOGIC,
                        EXIT
                } EXEC_PHASE;
        #endif
        }

        extern nthp::texture::Palette activePalette;

        class Script {
        public:

                struct Action {
                        Action() { varIndex = 0; boundKey = SDLK_UNKNOWN; }
                        uint32_t varIndex;
                        int boundKey;
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


                        nthp::texture::gTexture* textureBlock; // Context texture data.
                        size_t textureBlockSize;

                        nthp::entity::gEntity* entityBlock; // Context entity data
                        size_t entityBlockSize;

                        nthp::texture::Frame* frameBlock; // Context frame data.
                        size_t frameBlockSize;

                        nthp::script::scriptTriggerComplex currentTriggerConfig; // Just some data on the current stage.

                        Action* actionList; // Tracks keypresses; created with ACTION_DEFINE and ACTION_BIND to configure.
                        size_t  actionListSize;

                        short penColor; // stores a color as an index to the palette to draw primitives with the DRAW instruction.


                        bool isSuspended, changeStage; // Stage stuff.

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