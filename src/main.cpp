
#include "global.hpp"
#include "core.hpp"
#include "ray.hpp"
#include "position.hpp"
#include "palette.hpp"
#include "softwaretexture.hpp"


nthp::ray mouseRay;
nthp::vect32 mousePos;


nthp::EngineCore core(nthp::RenderRuleSet(800, 800, 1000, 1000, nthp::vectFixed(0,0)), "Testing Window", false, false);

void hEvents(SDL_Event* events) {
        switch(events->type) {
        case SDL_MOUSEBUTTONDOWN:
                if(events->button.button == SDL_BUTTON_LEFT) {

                }
                break;
        case SDL_KEYDOWN:
                if(events->key.keysym.sym == SDLK_ESCAPE) {
                        core.stop();
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



        SDL_Rect src = {0,0,20,20};
        SDL_Rect dst = {100,100,200,200};


        while(core.isRunning()) {

                core.handleEvents(hEvents);
               

                core.clear();

                if(SDL_RenderCopy(core.getRenderer(), texture.getTexture(), &src, &dst) < 0)
                        printf("%s\n", SDL_GetError());

                core.display();

        }


        return 0;
}