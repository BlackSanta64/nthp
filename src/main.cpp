
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
                        SDL_GetMouseState(&mousePos.x, &mousePos.y);
                        mouseRay.x1 = nthp::intToFixed(mousePos.x);
                        mouseRay.y1 = nthp::intToFixed(mousePos.y);
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

        uint8_t pixels[9] = {
                0, 0, 0,
                0, 1, 0,
                0, 0, 0
        };

        file.write((char*)&header, sizeof(header));
        file.write((char*)pixels, 9);
        file.close();
}





int main(int argv, char** argc) {


        printf("xScale = %lf\nyScale = %lf\n", nthp::fixedToDouble(core.p_coreDisplay.scaleFactor.x), nthp::fixedToDouble(core.p_coreDisplay.scaleFactor.y));
        {
                nthp::texture::Palette newPalette;
                newPalette.colorSet[0] = nthp::texture::Pixel({0,0,0,255}); // Black Opaque
                newPalette.colorSet[1] = nthp::texture::Pixel({255,255,255,255}); // White Opaque
                newPalette.colorSet[2] = nthp::texture::Pixel({0,0,0,0}); // Transparent
                newPalette.colorSet[3] = nthp::texture::Pixel({0,0,0,25}); // Transparent
                newPalette.colorSet[4] = nthp::texture::Pixel({0,0,0,50}); // Transparent
                newPalette.colorSet[5] = nthp::texture::Pixel({0,0,0,75}); // Transparent
                newPalette.colorSet[6] = nthp::texture::Pixel({0,0,0,100}); // Transparent
                newPalette.colorSet[7] = nthp::texture::Pixel({0,0,0,125}); // Transparent
                newPalette.colorSet[8] = nthp::texture::Pixel({0,0,0,150}); // Transparent
                newPalette.colorSet[9] = nthp::texture::Pixel({0,0,0,175}); // Transparent
                newPalette.colorSet[10] = nthp::texture::Pixel({0,0,0,200}); // Transparent
                newPalette.colorSet[11] = nthp::texture::Pixel({0,0,0,225}); // Transparent

                newPalette.exportPaletteToFile("newPalette.pal");
        }

        nthp::texture::Palette newPalette("newPalette.pal");
        nthp::texture::SoftwareTexture square("player.st", newPalette, core.getRenderer());

        nthp::texture::tools::generateSoftwareTextureFromImage("player.png", newPalette, "player.st");

        SDL_Rect testsrcRect = {0,0,20,20};
        SDL_Rect testdstRect = {0, 0, 800, 800};


 //      nthp::texture::tools::generateSoftwareTextureFromImage("player.png", newPalette, "player.st");
        nthp::rayCollision col;


        while(core.isRunning()) {

                core.handleEvents(hEvents);
               

        
              

             

                core.clear();

                SDL_RenderCopy(core.getRenderer(), square.getTexture(), &testsrcRect, &testdstRect);

                core.display();

        }


        return 0;
}