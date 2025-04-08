#pragma once


#include "s_node.hpp"
#include "s_instructions.hpp"
#include "e_entity.hpp"
#include "e_collision.hpp"
#include "gtexture.hpp"

#ifdef PM
        #ifndef DEBUG
	        #define DEBUG
        #endif
#endif


namespace nthp { 
namespace script {

                #define STAGEMEM_MAX 256

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

                struct ReturnStackEntry {
                        uint32_t sourceHeaderLocation;
                        uint32_t sourceDestination;
                };

        struct ScriptDataSet {
                        uint32_t globalMemBudget;

                        uint32_t* currentLabelBlock;
                        uint32_t currentLabelBlockSize;

                        uint32_t currentScriptHeaderLocation;

                        // Compiler-Evaluated.
                        //////////////////////////
                        nthp::script::Node* nodeSet;
                        size_t currentNode;
                        size_t nodeSetSize;


                        nthp::script::stdVarWidth* globalVarSet;
                        size_t varSetSize;
                        /////////////////////////


                        nthp::texture::gTexture* textureBlock; // Context texture data.
                        size_t textureBlockSize;

                        nthp::entity::gEntity* entityBlock; // Context entity data
                        size_t entityBlockSize;

                        nthp::texture::Frame* frameBlock; // Context frame data.
                        size_t frameBlockSize;

                        nthp::script::BlockMemoryEntry* blockData;
                        size_t blockDataSize;

                        Action* actionList; // Tracks keypresses; created with ACTION_DEFINE and ACTION_BIND to configure.
                        size_t  actionListSize;

                        ReturnStackEntry returnStack[256];
                        uint8_t stackPointer;

                        unsigned short penColor; // stores a color as an index to the palette to draw primitives with the DRAW instruction.

                        int currentMusicTrack;

                        bool isSuspended, changeStage; // Stage stuff.

        };
                static inline void cleanDataSet(ScriptDataSet* data) {
                        if(data->textureBlockSize > 0) delete[] data->textureBlock;
                        if(data->entityBlockSize > 0) {
                                for(auto i = 0; i < data->entityBlockSize; ++i) data->entityBlock[i].clean();
                                free(data->entityBlock);
                        }
                                if(data->frameBlockSize > 0) delete[] data->frameBlock;
                        if(data->globalMemBudget > 0) delete[] data->globalVarSet;
                        if(data->actionListSize > 0) delete[] data->actionList;
                        if(data->blockDataSize > 0) {
                                for(size_t i = 0; i < data->blockDataSize; ++i) {
                                        if(!(data->blockData[i].isFree)) free(data->blockData[i].data);
                                }
                
                                free(data->blockData);
                        }

                        if(data->nodeSetSize > 0) {
                                for(size_t i = 0; i < data->nodeSetSize; ++i) {
                                        if(data->nodeSet[i].access.size) { free(data->nodeSet[i].access.data); }
                                }

                                delete[] data->nodeSet;
                        }


                        memset(data, 0, sizeof(ScriptDataSet));
                }





                Script();
                Script(ScriptDataSet* dataSet, uint32_t headerLocation);

                int import(ScriptDataSet* dataSet, uint32_t headerLocation);

                int execute();
                int execute(uint32_t entryPoint);

                

                
                ScriptDataSet* getScriptData() { return data; }

                ~Script();
        private:
                mutable ScriptDataSet* data;
                

                uint32_t* localLabelBlock;
                uint32_t localLabelBlockSize;

                size_t localCurrentNode;
                uint32_t script_begin; // Points to the header of the corresponding linked object.

                bool inStageContext;
        };


}

}
