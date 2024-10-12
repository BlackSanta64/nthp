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

void handleStageActions(SDL_Event* event) {
        switch(event->type) {
                case SDL_KEYDOWN:
                        for(size_t i = 0; i < currentStage.getStageDataSet().actionListSize; ++i) {
                                if(event->key.keysym.sym == currentStage.getStageDataSet().actionList[i].boundKey) {
                                        
                                }
                        }
                        break;
        }
}


int main(int argv, char** argc) {
        // The DEBUG_INIT is called at the start of main, and DEBUG_CLOSE
        // is called after the destruction of the main core.

#ifdef DEBUG
        //NTHP_GEN_DEBUG_INIT(stdout);
        NTHP_GEN_DEBUG_INIT(fopen("debug.log", "w+"));
#endif
        { // The entire engine debug context.
                


                uint16_t maxFPS = 500;
                nthp::setMaxFPS(maxFPS);
                auto frameStart = SDL_GetTicks();

                nthp::script::CompilerInstance comp;
                if(comp.compileStageConfig("testStage.stg", "ts.cstg", true)) {

                        return -1;
                };




                const nthp::fixed_t cameraSpeed = nthp::doubleToFixed(0.5);

                

                if(currentStage.init()) return -1;

                
                while(nthp::core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        nthp::core.handleEvents();

                        currentStage.tick();
                        currentStage.logic();


                        nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                       
                        if(nthp::deltaTime < nthp::frameDelay) {
                                SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                nthp::deltaTime = nthp::frameDelay;
                        }
                }

                currentStage.exit();

        }

        return 0;
}
