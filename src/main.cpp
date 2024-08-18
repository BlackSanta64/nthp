
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

        NTHP_GEN_DEBUG_INIT(stdout);
        
        { // The entire engine debug context.
                

                nthp::setMaxFPS(120);

                nthp::EngineCore core(nthp::RenderRuleSet(800, 800, 500, 500, nthp::vectFixed(0,0)), "Testing Window", false, false);
                auto frameStart = SDL_GetTicks();


                nthp::texture::SoftwareTexture playerTexture;
                {
                        nthp::texture::Palette pal("genericPalette.pal");
                        playerTexture.generateTexture("player.st", &pal, core.getRenderer());
                }
                nthp::texture::Frame playerFrame;
                playerFrame.texture = playerTexture.getTexture();
                playerFrame.src = {0,0,20,20};

                nthp::entity::gEntity player;
                player.setRenderSize(nthp::vectFixed(nthp::intToFixed(10), nthp::intToFixed(10)));
                player.importFrameData(&playerFrame, 1, false);
                player.setCurrentFrame(0);

                nthp::fixed_t playerSpeed = nthp::intToFixed(2);
                const nthp::fixed_t acc = nthp::doubleToFixed(0.03);



                while(core.isRunning()) {
                        frameStart = SDL_GetTicks();

                        core.handleEvents(hEvents);
		
                        if(u)
                                player.move(nthp::vectFixed(0, -playerSpeed));
                        if(d)
                                player.move(nthp::vectFixed(0, playerSpeed));
                        if(l)
                                player.move(nthp::vectFixed(-playerSpeed, 0));
                        if(r)
                                player.move(nthp::vectFixed(playerSpeed, 0));

                        if(inc)
                                playerSpeed += acc;
                        if(dec)
                                playerSpeed -= acc;
                        

		        core.clear();
                        core.render(player.getUpdateRenderPacket(&core.p_coreDisplay));
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
