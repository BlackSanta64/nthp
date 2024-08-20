#include "global.hpp"
#include "core.hpp"
#include "ray.hpp"
#include "position.hpp"
#include "palette.hpp"
#include "softwaretexture.hpp"
#include "e_entity.hpp"

bool u,d,l,r,inc,dec;

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

        NTHP_GEN_DEBUG_INIT(fopen("debug.log", "w+"));
        
        { // The entire engine debug context.
                
                nthp::setMaxFPS(120);

                nthp::EngineCore core(nthp::RenderRuleSet(800, 800, 500, 500, nthp::vectFixed(0,0)), "Testing Window", false, false);
                auto frameStart = SDL_GetTicks();

                nthp::texture::Palette pal("genericPalette.pal");
                nthp::texture::SoftwareTexture tux("tux.st", &pal, core.getRenderer());

                PRINT_DEBUG("Generated TUX.ST.\n");

                nthp::texture::Frame tuxFrame;
                tuxFrame.texture = tux.getTexture();
                tuxFrame.src = {0, 0, 764, 910};

                nthp::entity::gEntity e_tux;
                e_tux.importFrameData(&tuxFrame, 1, false);
                e_tux.setRenderSize(nthp::vectFixed(nthp::intToFixed(500), nthp::intToFixed(500)));
                e_tux.setPosition(nthp::vectFixed(0,0));

                
                while(core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        core.handleEvents(hEvents);

                    

		        core.clear();
                        core.render(e_tux.getUpdateRenderPacket(&core.p_coreDisplay));
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
