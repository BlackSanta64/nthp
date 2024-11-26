#include "core.hpp"



nthp::EngineCore::EngineCore(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering) {
        window = nullptr;
        renderer = nullptr;
        running = false;
        initSuccess = false;

        if(this->init(settings, title, fullscreen, softwareRendering)) { }
}

int nthp::EngineCore::init(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering) {
        p_coreDisplay = settings;

        // If already initialized, skip the init.
        if(!initSuccess) {
                PRINT_DEBUG("Initializing SDL binaries...\t");

                auto flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
                
                if(SDL_Init(flags) != 0) {
                        FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
                }

        #if USE_SDLIMG == 1
                auto imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;

                if(IMG_Init(imgFlags) != imgFlags) {
                        FATAL_PRINT(nthp::FATAL_ERROR::SDL_Failure, SDL_GetError());
                }
        #endif
                NOVERB_PRINT_DEBUG("done.\n");
        }
        PRINT_DEBUG("Setting up window and renderer...\t");

        int fullscreenFlag = 0;
        if(fullscreen) {
                fullscreenFlag = SDL_WINDOW_FULLSCREEN;
        }

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

        SDL_SetRenderDrawColor(renderer, DEFAULT_RENDER_COLOR);
        running = true;
	
	// This ensures the correct render resolution context when calculating scale factors.
	// If the requested resolution is too small (or too large), SDL will correct the resolution to
	// match the aspect ratio of the display and capabilities of the graphics card. Querying for them
        // might seem redundant, but trust bro.
	{
		int w, h;
		SDL_GetRendererOutputSize(renderer, &w, &h);
		p_coreDisplay = nthp::RenderRuleSet(w, h, settings.tunitResolution_x, settings.tunitResolution_y, settings.cameraWorldPosition);
	}


        initSuccess = true;
        NOVERB_PRINT_DEBUG("done.\n\n");

        return 0;
}


void nthp::EngineCore::handleEvents() {
        int x,y;
        SDL_GetMouseState(&x, &y);
        nthp::mousePosition = nthp::generateWorldPosition(nthp::vectGeneric(x, y), &p_coreDisplay);
        nthp::mousePosition -= p_coreDisplay.cameraWorldPosition;

        while(SDL_PollEvent(&eventList)) {
                switch(eventList.type) {
                case SDL_QUIT:
                        running = false;
                        break;
                }
        }
}

void nthp::EngineCore::handleEvents(void (*handler)(SDL_Event*)) {
        int x,y;
        SDL_GetMouseState(&x, &y);
        nthp::mousePosition = nthp::generateWorldPosition(nthp::vectGeneric(x, y), &p_coreDisplay);
        nthp::mousePosition -= p_coreDisplay.cameraWorldPosition;

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


int nthp::EngineCore::render(nthp::RenderPacket packet) {
        switch(packet.state) {
                case nthp::RenderPacket::C_OPERATE::VALID:
                        {
                                vectGeneric offset = nthp::generatePixelPosition(nthp::worldPosition(p_coreDisplay.cameraWorldPosition.x, p_coreDisplay.cameraWorldPosition.y), &p_coreDisplay);
                                packet.dstRect.x += offset.x;
                                packet.dstRect.y += offset.y;
                                return SDL_RenderCopy(renderer, packet.texture, packet.srcRect, &packet.dstRect);
                        }
                        break;
                case nthp::RenderPacket::C_OPERATE::ABSOLUTE:
                        return SDL_RenderCopy(renderer, packet.texture, packet.srcRect, &packet.dstRect);
                        break;


                case nthp::RenderPacket::C_OPERATE::INVALID:
                        PRINT_DEBUG_WARNING("Reading invalid render call. Ensure target texture is generated.\n");
                        return -1;
                        break;
                default:
                        break;
        }

        return 0;
}

void nthp::EngineCore::setWindowRenderSize(int x, int y) {
        if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
                SDL_DisplayMode mode;
                SDL_GetWindowDisplayMode(window, &mode);
                
                // For some fucking reason, SDL Can't resize a window while in fullscreen,
                // Even when tweaking the DisplayMode. :/
                SDL_SetWindowFullscreen(window, 0);
                mode.w = x;
                mode.h = y;

                if(SDL_SetWindowDisplayMode(window, &mode) < 0) {
                        PRINT_DEBUG_ERROR("%s", SDL_GetError());
                }
                SDL_GetWindowDisplayMode(window, &mode);

                p_coreDisplay.pxlResolution_x = mode.w;
                p_coreDisplay.pxlResolution_y = mode.h;

                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }
        else {
                SDL_SetWindowSize(window, x, y);
		#ifdef LINUX // GETWINDOWSIZEINPIXELS is not a valid SDLcall on linux. Although not exactly the same, works in most cases.
	                SDL_GetWindowSize(window, &x, &y);
		#endif

		#ifdef WINDOWS
			SDL_GetWindowSizeInPixels(window, &x, &y);
		#endif

		
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

                p_coreDisplay.pxlResolution_x = x;
                p_coreDisplay.pxlResolution_y = y;
        }

        p_coreDisplay.updateScaleFactor();
}

void nthp::EngineCore::setVirtualRenderScale(nthp::fixed_t x, nthp::fixed_t y) {
        p_coreDisplay.tunitResolution_x = x;
        p_coreDisplay.tunitResolution_y = y;

        p_coreDisplay.updateScaleFactor();
}



int nthp::EngineCore::cleanup() {
        PRINT_DEBUG("Destroying Core and cleaning up...  ");

        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);

        NOVERB_PRINT_DEBUG("done.\n");

        return 0;
}

nthp::EngineCore::~EngineCore() {
        cleanup();

        SDL_Quit();
#if USE_SDLIMG == 1
        IMG_Quit();
#endif

        initSuccess = false;

#ifdef DEBUG
        NTHP_GEN_DEBUG_CLOSE();
#endif

}
