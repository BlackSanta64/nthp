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
#ifdef DEBUG
  //    NTHP_GEN_DEBUG_INIT(stdout);
        NTHP_GEN_DEBUG_INIT(fopen("debug.log", "w+"));
#endif
        { // The entire engine debug context.
                
              

                uint16_t maxFPS = 120;
                nthp::setMaxFPS(maxFPS);
                auto frameStart = SDL_GetTicks();

                nthp::entity::gEntity tux;

                nthp::script::CompilerInstance comp;
                comp.compileSourceFile("test.thp", "test.thpcs");

                nthp::script::Script test("test.thpcs");
                test.execute();
                

                

        

                tux.importFrameData(test.getScriptData()->frameBlock, 1, false);
                tux.setCurrentFrame(0);
                tux.setRenderSize(nthp::vectFixed(nthp::intToFixed(500), nthp::intToFixed(500)));
                tux.setPosition(nthp::vectFixed(0,0));

                printf("sf = %lf %lf\n", nthp::fixedToDouble(nthp::core.p_coreDisplay.scaleFactor.x), nthp::fixedToDouble(nthp::core.p_coreDisplay.scaleFactor.y));
                const nthp::fixed_t cameraSpeed = nthp::doubleToFixed(0.5);

                printf("val = %lf\n", nthp::fixedToDouble(test.getScriptData()->varSet[0]));

                
                while(nthp::core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        nthp::core.handleEvents(hEvents);
                        if(u) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.y += nthp::f_fixedProduct(cameraSpeed, nthp::deltaTime);
                        }
                        if(d) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.y -= nthp::f_fixedProduct(cameraSpeed, nthp::deltaTime);
                        }
                        if(l) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.x += nthp::f_fixedProduct(cameraSpeed, nthp::deltaTime);
                        }
                        if(r) {
                                nthp::core.p_coreDisplay.cameraWorldPosition.x -= nthp::f_fixedProduct(cameraSpeed, nthp::deltaTime);
                        }

                        if(inc) {
                                ++maxFPS;
                                nthp::setMaxFPS(maxFPS);
                        }
                        if(dec) {
                                --maxFPS;
                                nthp::setMaxFPS(maxFPS);
                        }



		        nthp::core.clear();
                        
                        nthp::core.render(tux.getUpdateRenderPacket(&nthp::core.p_coreDisplay));

                        nthp::core.display();

                        nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                       
                        if(nthp::deltaTime < nthp::frameDelay) {
                                SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                nthp::deltaTime = nthp::frameDelay;
                        }
                }


        }

#ifdef DEBUG
        NTHP_GEN_DEBUG_CLOSE();
#endif
        return 0;
}
