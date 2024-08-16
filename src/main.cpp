
#include "global.hpp"
#include "core.hpp"
#include "ray.hpp"
#include "position.hpp"
#include "palette.hpp"
#include "softwaretexture.hpp"
#include "e_entity.hpp"


nthp::vect32 mousePos;
nthp::EngineCore* globalCore = nullptr;



void hEvents(SDL_Event* events) {
        switch(events->type) {
        case SDL_KEYDOWN: 
		if(events->key.keysym.sym == SDLK_TAB) {
			globalCore->stop();
		}
                break;
	default:
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

        NTHP_GEN_DEBUG_INIT(fopen("debugLog.log", "w+"));
        
        { // The entire engine context.


                nthp::EngineCore core(nthp::RenderRuleSet(960, 540, 2000, 2000, nthp::vectFixed(0,0)), "Testing Window", false, false);
                globalCore = &core;

                nthp::texture::Palette pal("genericPalette.pal");
                pal.colorSet[nthp::texture::PaletteFileSize - 1].R = 255;
                pal.colorSet[nthp::texture::PaletteFileSize - 1].G = 255;
                pal.colorSet[nthp::texture::PaletteFileSize - 1].B = 255;

                pal.exportPaletteToFile("genericPalette.pal");

      	        nthp::texture::SoftwareTexture texture("wall.st", &pal, core.getRenderer());
	        nthp::entity::gEntity test, test2;

                nthp::texture::Frame frames;
                frames.src = {0,0,100,76};
                frames.texture = texture.getTexture();

                test.importFrameData(&frames, 1, false);
                test.setRenderSize(nthp::vectFixed(nthp::intToFixed(200), nthp::intToFixed(200)));
	        test.setHtiboxSize(nthp::vectFixed(nthp::intToFixed(180), nthp::intToFixed(180)));
	        test.setHitboxOffset(nthp::vectFixed(nthp::intToFixed(10), nthp::intToFixed(10)));

                test.setCurrentFrame(0);

	        test2.importFrameData(&frames, 1, false);
	        test2.setRenderSize(nthp::vectFixed(nthp::intToFixed(300), nthp::intToFixed(300)));
	        test2.setHtiboxSize(nthp::vectFixed(nthp::intToFixed(300), nthp::intToFixed(300)));
	        test2.setCurrentFrame(0);

	        test2.setPosition(nthp::vectFixed(nthp::intToFixed(1000), nthp::intToFixed(1000)));

                while(core.isRunning()) {
		        SDL_Delay(10);


                        core.handleEvents(hEvents);
		
		        SDL_GetMouseState(&mousePos.x, &mousePos.y);

		        test.setPosition(nthp::generateWorldPosition(nthp::vect64(mousePos.x, mousePos.y), &core.p_coreDisplay));
		        if(nthp::entity::checkRectCollision(test.getHitbox(), test2.getHitbox())) {
		        	SDL_SetRenderDrawColor(core.getRenderer(), 100, 255, 50, 255);
		        }

		        core.clear();
                        core.render(test.getUpdateRenderPacket(&core.p_coreDisplay));
		        core.render(test2.getUpdateRenderPacket(&core.p_coreDisplay));
                        core.display();
		
		        SDL_SetRenderDrawColor(core.getRenderer(), 144, 144, 144, 255);

                }
        } // The entire engine context.


        NTHP_GEN_DEBUG_CLOSE();
        return 0;
}
