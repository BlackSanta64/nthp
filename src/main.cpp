
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

        mouseRay.x1 = 0;
        mouseRay.y1 = 0;

        int x = 5002;

        nthp::ray testRay;
        testRay.x1 = nthp::intToFixed(120);
        testRay.y1 = nthp::intToFixed(x);

        testRay.x2 = nthp::intToFixed(732);
        testRay.y2 = nthp::intToFixed(633);

        printf("xScale = %lf\nyScale = %lf\n", nthp::fixedToDouble(core.p_coreDisplay.scaleFactor.x), nthp::fixedToDouble(core.p_coreDisplay.scaleFactor.y));

        nthp::texture::Palette newPalette("palette.pal");
        nthp::texture::SoftwareTexture square("test.st", newPalette, core.getRenderer());

        SDL_Rect testsrcRect = {0,0,3,3};
        SDL_Rect testdstRect = {100, 100, 30, 30};


        while(core.isRunning()) {

                core.handleEvents(hEvents);
                SDL_GetMouseState(&mousePos.x, &mousePos.y);
                mouseRay.x2 = nthp::intToFixed(mousePos.x);
                mouseRay.y2 = nthp::intToFixed(mousePos.y);

        

                if(nthp::rayColliding(mouseRay, testRay).isColliding) {
                        SDL_SetRenderDrawColor(core.getRenderer(), 120, 255, 120, SDL_ALPHA_OPAQUE);
                }

             

                core.clear();
                SDL_SetRenderDrawColor(core.getRenderer(), 255, 100, 100, SDL_ALPHA_OPAQUE);

                SDL_RenderDrawLine(core.getRenderer(), nthp::fixedToInt(mouseRay.x1), nthp::fixedToInt(mouseRay.y1), nthp::fixedToInt(mouseRay.x2), nthp::fixedToInt(mouseRay.y2));
                SDL_RenderDrawLine(core.getRenderer(), nthp::fixedToInt(testRay.x1), nthp::fixedToInt(testRay.y1), nthp::fixedToInt(testRay.x2), nthp::fixedToInt(testRay.y2));
                SDL_RenderCopy(core.getRenderer(), square.getTexture(), &testsrcRect, &testdstRect);

                core.display();
                SDL_SetRenderDrawColor(core.getRenderer(), 144, 144, 144, SDL_ALPHA_OPAQUE);
        }


        return 0;
}