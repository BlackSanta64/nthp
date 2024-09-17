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
nthp::EngineCore* targetCore;

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
                                targetCore->stop();
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

    //          NTHP_GEN_DEBUG_INIT(stdout);
              NTHP_GEN_DEBUG_INIT(fopen("debug.log", "w+"));
        
        { // The entire engine debug context.
                
                nthp::setMaxFPS(120);

                nthp::EngineCore core(nthp::RenderRuleSet(960, 540, 500, 500, nthp::vectFixed(0,0)), "Testing Window", false, false);
                targetCore = &core;
                auto frameStart = SDL_GetTicks();

                nthp::script::CompilerInstance comp;
                comp.compileSourceFile("test.thp", "test.thpcs");

                nthp::script::Script test("test.thpcs");
                test.execute();

                printf("sqrt = %lf\n", nthp::fixedToDouble(test.getVar(4)));

                
                while(core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        core.handleEvents(hEvents);

                        test.execute();

		        core.clear();

                        core.display();

                        nthp::deltaTime = nthp::intToFixed(SDL_GetTicks() - frameStart);
                       
                        if(nthp::deltaTime < nthp::frameDelay) {
                                SDL_Delay(nthp::fixedToInt(nthp::frameDelay - nthp::deltaTime));
                                nthp::deltaTime = nthp::frameDelay;
                        }
                }


        }


        NTHP_GEN_DEBUG_CLOSE();
        return 0;
}
