#include "global.hpp"
#include "core.hpp"
#include "ray.hpp"
#include "position.hpp"
#include "palette.hpp"
#include "softwaretexture.hpp"
#include "e_entity.hpp"
#include "st_compress.hpp"
#include "s_compiler.hpp"


bool u,d,l,r,inc,dec;
nthp::EngineCore nthp::core;

nthp::script::stage::Stage currentStage;

int nthp::runtimeBehaviour(int argv, char** argc) {
        // The DEBUG_INIT is called at the start of main, and DEBUG_CLOSE
        // is called after the destruction of the main core.

#ifdef DEBUG
        NTHP_GEN_DEBUG_INIT(stdout);
       //NTHP_GEN_DEBUG_INIT(fopen("debug.log", "w+"));
#endif
        { // The entire engine debug context.
                {
                        std::string init = "prog";
                        if(argv > 1) {
                                std::string outputName = argc[1];
                                if(outputName == "-jit") {
                                        outputName = argc[2];
                                        outputName += ".cstg";
                                        nthp::script::CompilerInstance comp;
                                        comp.compileStageConfig(argc[2], outputName.c_str(), false, false);
                                }
                                init = outputName;
                        }

                        memcpy(nthp::script::stageMemory, init.c_str(), init.size());
                        nthp::script::stageMemory[init.size()] = '\000';

                }
                auto frameStart = SDL_GetTicks();


                // Anyone would agree an infinite loop here is acceptable.
                while(true) {
                        if(currentStage.loadStage(nthp::script::stageMemory)) return -1;
                        memset(nthp::script::stageMemory, 0, STAGEMEM_MAX);

                        // Init phase.
                        if(currentStage.init()) return -1;
  
                        
                        while(nthp::core.isRunning() && (!currentStage.data.changeStage)) {
                                frameStart = SDL_GetTicks();

                                currentStage.handleEvents();

                                // Tick phase.
                                currentStage.tick();
                                currentStage.logic();


                                nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                        
                                if(nthp::deltaTime < nthp::frameDelay) {
                                        SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                        nthp::deltaTime = nthp::frameDelay;
                                }
                                currentStage.data.globalVarSet[DELTATIME_GLOBAL_INDEX] = nthp::deltaTime;
                        }

                        // Exit Phase
                        if(currentStage.exit()) return -1;

                        // If CHANGESTAGE is false after the exit phase, then the core must've been stopped,
                        // so exit the program. Otherwise, redo the init phase with the new stage. 
                        if(currentStage.data.changeStage) {
                                currentStage.data.changeStage = false;
                                // StageMemory has been set to the new filename.
                                
                                continue;
                        }
                        else {
                                break;
                        }
                }
                
        }

        return 0;
}




int main(int argv, char** argc) {
        return nthp::runtimeBehaviour(argv, argc);
}
