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

void hEvents(SDL_Event* event) {
        switch(event->type) {
                case SDL_KEYDOWN:
                        if(event->key.keysym.sym == SDLK_UP)
                                inc = true;
                        if(event->key.keysym.sym == SDLK_DOWN)
                                dec = true;       

                        if(event->key.keysym.sym == SDLK_w) {
                                u = true;
                        }
                        if(event->key.keysym.sym == SDLK_s) {
                                d = true;
                        }
                        if(event->key.keysym.sym == SDLK_a) {
                                l = true;
                        }
                        if(event->key.keysym.sym == SDLK_d) {
                                r = true;
                        }
                        if(event->key.keysym.sym == SDLK_TAB) {
                                nthp::core.stop();
                        }
                        if(event->key.keysym.sym == SDLK_q) {
                                nthp::core.setWindowRenderSize(1920, 1080);
                        }
                        if(event->key.keysym.sym == SDLK_e) {
                                nthp::core.setWindowRenderSize(960, 540);
                        }
                        break;

                case SDL_KEYUP:
                        if(event->key.keysym.sym == SDLK_UP)
                                inc = false;
                        if(event->key.keysym.sym == SDLK_DOWN)
                                dec = false;       


                        if(event->key.keysym.sym == SDLK_w) {
                                u = false;

                        }
                        if(event->key.keysym.sym == SDLK_s) {
                                d = false;
                        }
                        if(event->key.keysym.sym == SDLK_a) {
                                l = false;
                        }
                        if(event->key.keysym.sym == SDLK_d) {
                                r = false;
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


                nthp::script::stage::Stage testStage("ts.cstg");

                const nthp::fixed_t cameraSpeed = nthp::doubleToFixed(0.5);


                if(testStage.init()) return -1;
                

                
                while(nthp::core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        nthp::core.handleEvents(hEvents);
                        printf("%d %d\n", nthp::fixedToInt(nthp::mousePosition.x), nthp::fixedToInt(nthp::mousePosition.y));
                        if(u) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.y += nthp::f_fixedProduct(nthp::intToFixed(1), nthp::deltaTime);
                        }
                        if(d) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.y -= nthp::f_fixedProduct(nthp::intToFixed(1), nthp::deltaTime);
                        }
                        if(l) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.x += nthp::f_fixedProduct(nthp::intToFixed(1), nthp::deltaTime);
                        }
                        if(r) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.x -= nthp::f_fixedProduct(nthp::intToFixed(1), nthp::deltaTime);
                        }
                        if(inc) {
                                nthp::core.setVirtualRenderScale(nthp::core.p_coreDisplay.tunitResolution_x + nthp::intToFixed(1), nthp::core.p_coreDisplay.tunitResolution_y + nthp::intToFixed(1));
                        }
                        if(dec) {
                                nthp::core.setVirtualRenderScale(nthp::core.p_coreDisplay.tunitResolution_x - nthp::intToFixed(1), nthp::core.p_coreDisplay.tunitResolution_y - nthp::intToFixed(1));
                        }

                        testStage.tick();
                        testStage.logic();


		        nthp::core.clear();
                        
                        nthp::core.render(testStage.getScript(0).getScriptData()->entityBlock[0].getUpdateRenderPacket(&nthp::core.p_coreDisplay));
                        nthp::core.render(testStage.getScript(0).getScriptData()->entityBlock[1].abs_getRenderPacket(&nthp::core.p_coreDisplay));

                        nthp::core.display();

                        nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                       
                        if(nthp::deltaTime < nthp::frameDelay) {
                                SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                nthp::deltaTime = nthp::frameDelay;
                        }
                }

                testStage.exit();

        }

        return 0;
}
