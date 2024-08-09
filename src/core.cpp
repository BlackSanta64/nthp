#include "core.hpp"


nthp::RenderRuleSet::RenderRuleSet() { 
        pxlResolution_x = 0;
        pxlResolution_y = 0;
        tunitResolution_x = 0;
        tunitResolution_y = 0;
}

nthp::RenderRuleSet::RenderRuleSet(FIXED_TYPE x, FIXED_TYPE y, FIXED_TYPE tx, FIXED_TYPE ty, vectFixed cameraPosition) {
        pxlResolution_x = x;
        pxlResolution_y = y;
        tunitResolution_x = tx;
        tunitResolution_y = ty;

        cameraWorldPosition = cameraPosition;
        scaleFactor.x = nthp::f_fixedQuotient(nthp::intToFixed(pxlResolution_x), nthp::intToFixed(tunitResolution_x));
	scaleFactor.y = nthp::f_fixedQuotient(nthp::intToFixed(pxlResolution_y), nthp::intToFixed(tunitResolution_y));
}

void nthp::RenderRuleSet::updateRuleset(const nthp::RenderRuleSet& newSet) {
	pxlResolution_x = newSet.pxlResolution_x;
        pxlResolution_y = newSet.pxlResolution_y;
        tunitResolution_x = newSet.tunitResolution_x;
        tunitResolution_y = newSet.tunitResolution_y;

	scaleFactor.x = nthp::f_fixedQuotient(nthp::intToFixed(pxlResolution_x), nthp::intToFixed(tunitResolution_x));
	scaleFactor.y = nthp::f_fixedQuotient(nthp::intToFixed(pxlResolution_y), nthp::intToFixed(tunitResolution_y));
}




nthp::EngineCore::EngineCore(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering) {
        p_coreDisplay = settings;

        PRINT_DEBUG("Initializing SDL binaries...\t");

        auto flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
        int fullscreenFlag = 0;
        if(SDL_Init(flags) != 0) {
                FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
        }
        if(fullscreen) {
                fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

#if USE_SDLIMG == 1
        auto imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;

        if(IMG_Init(imgFlags) != imgFlags) {
                FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
        }
#endif
        PRINT_DEBUG("done.\nSetting up window and renderer...\t");

        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_coreDisplay.pxlResolution_x, p_coreDisplay.pxlResolution_y, fullscreenFlag);
        if(window == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
        }

        if(softwareRendering)
                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        else
                renderer = SDL_CreateRenderer(window, -1, 0);

        if(renderer == NULL) {
                FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
        }

        PRINT_DEBUG("done.\nConfiguring defaults...\t");

        SDL_SetRenderDrawColor(renderer, 144, 144, 144, SDL_ALPHA_OPAQUE);
        running = true;

        PRINT_DEBUG("done.\n\n");
}



void nthp::EngineCore::handleEvents() {
        while(SDL_PollEvent(&eventList)) {
                switch(eventList.type) {
                case SDL_QUIT:
                        running = false;
                        break;
                }
        }
}

void nthp::EngineCore::handleEvents(void (*handler)(SDL_Event*)) {
        while(SDL_PollEvent(&eventList)) {
                switch(eventList.type) {
                case SDL_QUIT:
                        running = false;
                        break;

                default:
                        handler(&eventList);
                        break;
                }
        }
}



void nthp::EngineCore::display() {
        SDL_RenderPresent(renderer);
}

void nthp::EngineCore::clear() {
        SDL_RenderClear(renderer);
}

void nthp::EngineCore::stop() {
        running = false;
}


void nthp::EngineCore::render(nthp::RenderPacket packet) {
        SDL_RenderCopy(renderer, packet.texture, packet.srcRect, &packet.dstRect);
}




nthp::EngineCore::~EngineCore() {
        PRINT_DEBUG("Destroying core [%p]...\t", this);

        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);

        SDL_Quit();
#if USE_SDLIMG == 1
        IMG_Quit();
#endif

        PRINT_DEBUG("done.\n");
}