#pragma once
#include "global.hpp"



namespace nthp {

        struct RenderPacket {
                SDL_Texture* texture;
                SDL_Rect* srcRect;
                SDL_Rect dstRect;

                enum C_OPERATE { VALID, INVALID } state;
        };
        static nthp::RenderPacket generateRenderPacket(SDL_Texture* texture, SDL_Rect* srcRect, SDL_Rect dst, nthp::RenderPacket::C_OPERATE s) {
                return {texture, srcRect, dst, s};
        }

        struct RenderRuleSet {
        public:
		RenderRuleSet();
                RenderRuleSet(FIXED_TYPE x, FIXED_TYPE y, fixed_t tx, fixed_t ty, vectFixed cameraPosition);

		FIXED_TYPE pxlResolution_x;
		FIXED_TYPE pxlResolution_y;
		fixed_t tunitResolution_x;
		fixed_t tunitResolution_y;
                vectFixed scaleFactor;

		vectFixed cameraWorldPosition;

        private:
		vector<FIXED_TYPE> cameraPixelPosition;
  
        };


        class EngineCore {
        public:
                EngineCore() { window = nullptr; renderer = nullptr; running = false; };
                EngineCore(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering);

                int init(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering);

                void handleEvents();
                void handleEvents(void (*handler)(SDL_Event*));

                int render(nthp::RenderPacket packet);
                void clear();
                void display();
                void stop();

                void setWindowRenderSize(int x, int y);
                void setVirtualRenderScale(float x, float y);

                inline bool isRunning() { return running; }

                nthp::RenderRuleSet p_coreDisplay;
                SDL_Event eventList;

                inline SDL_Window* getWindow() { return window; }
                inline SDL_Renderer* getRenderer() { return renderer; }



                ~EngineCore();
        private:
                SDL_Window* window;
                SDL_Renderer* renderer;
        
                bool running;

        };

        extern nthp::EngineCore core;


}

