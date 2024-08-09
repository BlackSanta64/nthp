#pragma once

#include "softwaretexture.hpp"
#include "position.hpp"
#include "e_collision.hpp"

namespace nthp {
        namespace entity {

        class gEntity {
        public:
                gEntity();

                virtual const nthp::RenderPacket getUpdateRenderPacket(nthp::RenderRuleSet* context) final;
                virtual const nthp::RenderPacket abs_getRenderPacket(nthp::RenderRuleSet* context) final;

                void importFrameData(nthp::texture::Frame* data, size_t size, bool native);
                void setRenderSize(nthp::vectFixed size);

                void setPosition(nthp::vectFixed newPos);
                void move(nthp::vectFixed offset);

                inline nthp::vectFixed getRenderSize() { return renderSize; }
                inline nthp::worldPosition getPosition() { return wPosition; }

                void setCurrentFrame(size_t cf);
                inline size_t getCurrentFrame() { return currentFrame; }


                ~gEntity();
        protected:
                nthp::texture::Frame* frameData;
                size_t frameSize;
                bool frameDataNative;
                
                size_t currentFrame;
                nthp::vectFixed renderSize;

                nthp::worldPosition wPosition;
                nthp::entity::cRect hitbox;


        };

        }

}