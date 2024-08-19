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


	float xs, ys;
	xs = (float)pxlResolution_x / (float)tunitResolution_x;
	ys = (float)pxlResolution_y / (float)tunitResolution_y;


	scaleFactor.x = nthp::doubleToFixed(xs);
	scaleFactor.y = nthp::doubleToFixed(ys);
}

void nthp::RenderRuleSet::updateRuleset(const nthp::RenderRuleSet& newSet) {
	pxlResolution_x = newSet.pxlResolution_x;
        pxlResolution_y = newSet.pxlResolution_y;
        tunitResolution_x = newSet.tunitResolution_x;
        tunitResolution_y = newSet.tunitResolution_y;

	float xs, ys;
	xs = (float)pxlResolution_x / (float)tunitResolution_x;
	ys = (float)pxlResolution_y / (float)tunitResolution_y;



	scaleFactor.x = nthp::doubleToFixed(xs);
	scaleFactor.y = nthp::doubleToFixed(ys);

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
        NOVERB_PRINT_DEBUG("done.\n");
        PRINT_DEBUG("Setting up window and renderer...\t");

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
        NOVERB_PRINT_DEBUG("done.\n");
        PRINT_DEBUG("Configuring defaults...\t");

        SDL_SetRenderDrawColor(renderer, 144, 144, 144, SDL_ALPHA_OPAQUE);
        running = true;
	
	// This ensures the correct render resolution context when calculating scale factors.
	// If the requested resolution is too small (or too large), SDL will correct the resolution to
	// match the aspect ratio of the display and capabilities of the graphics card. Querying for them
        // might seem redundant, but trust bro.
	{
		int w, h;
		SDL_GetRendererOutputSize(renderer, &w, &h);
		p_coreDisplay.updateRuleset(nthp::RenderRuleSet(w, h, settings.tunitResolution_x, settings.tunitResolution_y, settings.cameraWorldPosition));
	}
        NOVERB_PRINT_DEBUG("done.\n\n");
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

        NOVERB_PRINT_DEBUG("done.\n");
}
