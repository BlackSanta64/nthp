
#include "global.hpp"
#include "core.hpp"
#include "ray.hpp"
#include "position.hpp"
#include "palette.hpp"
#include "softwaretexture.hpp"
#include "e_entity.hpp"


nthp::vect32 mousePos;

bool increase = false, decrease = false;

nthp::EngineCore core(nthp::RenderRuleSet(1920, 1080, 1000, 1000, nthp::vectFixed(0,0)), "Testing Window", true, false);
nthp::entity::gEntity test;

void hEvents(SDL_Event* events) {
        switch(events->type) {
        case SDL_KEYDOWN:
                if(events->key.keysym.sym == SDLK_UP) {
                        increase = true;
                }
                if(events->key.keysym.sym == SDLK_DOWN) {
                        decrease = true;
                }
		if(events->key.keysym.sym == SDLK_TAB) {
			core.stop();
		}
                break;
        case SDL_KEYUP:
                if(events->key.keysym.sym == SDLK_UP) {
                        increase = false;
                }
                if(events->key.keysym.sym == SDLK_DOWN) {
                        decrease = false;
                }
                break;
        }
}


void createSTSquare() {
        std::fstream file("test.st", std::ios::out | std::ios::binary);
        nthp::texture::SoftwareTexture::software_texture_header header;
        header.signature = nthp::texture::STheaderSignature;

        header.x = 3;
        header.y = 3;

        NTHPST_COLOR_WIDTH pixels[9] = {
                15, 15, 15,
                15, 31, 15,
                15, 15, 15
        };

        file.write((char*)&header, sizeof(header));
        file.write((char*)pixels, sizeof(NTHPST_COLOR_WIDTH) * 9);
        file.close();
}





int main(int argv, char** argc) {

      
        nthp::texture::Palette pal("palette.pal");


        nthp::texture::SoftwareTexture texture("player.st", &pal, core.getRenderer());

        
        nthp::texture::Frame frames;
        frames.src = {0,0,20,20};
        frames.texture = texture.getTexture();

        test.importFrameData(&frames, 1, false);
        test.setRenderSize(nthp::vectFixed(nthp::intToFixed(200), nthp::intToFixed(200)));
        test.setCurrentFrame(0);


        while(core.isRunning()) {

                core.handleEvents(hEvents);
           

		core.clear();

                core.render(test.getUpdateRenderPacket(&core.p_coreDisplay));

                core.display();

        }


        return 0;
}
