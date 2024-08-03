#pragma once
#include "global.hpp"



namespace nthp {

        struct RenderPacket {
                SDL_Texture* texture;
                SDL_Rect* srcRect;
                SDL_Rect* dstRect;
        };
        #define Generate_RenderPacket(texture, srceRect, destRect) RenderPacket({texture, srcRect, dstRect})


        struct RenderRuleSet {
        public:
		RenderRuleSet();
                RenderRuleSet(FIXED_TYPE x, FIXED_TYPE y, FIXED_TYPE tx, FIXED_TYPE ty, vectFixed cameraPosition);

		void updateRuleset(const nthp::RenderRuleSet& newSet);

		FIXED_TYPE pxlResolution_x;
		FIXED_TYPE pxlResolution_y;
		FIXED_TYPE tunitResolution_x;
		FIXED_TYPE tunitResolution_y;
                vectFixed scaleFactor;

		vectFixed cameraWorldPosition;

        private:
		vector<FIXED_TYPE> cameraPixelPosition;
  
        };


        class EngineCore {
        public:
                EngineCore(nthp::RenderRuleSet settings, const char* title, bool fullscreen, bool softwareRendering);
                void render(nthp::RenderPacket packet);

                void handleEvents();
                void handleEvents(void (*handler)(SDL_Event*));

                void clear();
                void display();
                void stop();

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


}

